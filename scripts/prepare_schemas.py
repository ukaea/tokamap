"""Render schemas using a release tag or the next provisional minor version."""

import argparse
import json
from pathlib import Path
import re
import subprocess


VERSION_PATTERN = r"v?[0-9]+\.[0-9]+(?:\.[0-9]+)?(?:-[0-9A-Za-z.-]+)?(?:\+[0-9A-Za-z.-]+)?"
DOCS_URL = "https://ukaea.github.io/tokamap"


def git(root, *args):
    return subprocess.check_output(["git", *args], cwd=root).decode().strip()


def schema_version(root, version=None, develop=False):
    if version is not None:
        if develop:
            raise ValueError("Use either a release version or --develop")
        if not re.fullmatch(VERSION_PATTERN, version):
            raise ValueError("Release version must be a version tag, such as 0.3.2")
        return version, False

    # Source distributions retain the exact schema version without needing Git.
    saved_version = root / ".schema-version.json"
    if saved_version.is_file() and not develop:
        metadata = json.loads(saved_version.read_text())
        if not re.fullmatch(VERSION_PATTERN, metadata["version"]):
            raise ValueError("Invalid schema version in source distribution")
        return metadata["version"], metadata["development"]

    tags = git(root, "tag", "--points-at", "HEAD").splitlines()
    release_tags = sorted(tag for tag in tags if re.fullmatch(VERSION_PATTERN, tag))
    if not develop and release_tags:
        if len(release_tags) != 1:
            raise ValueError("Multiple version tags at HEAD; specify --version")
        return release_tags[0], False

    previous = git(root, "describe", "--tags", "--abbrev=0", "--match", "[0-9]*", "--match", "v[0-9]*")
    if not re.fullmatch(VERSION_PATTERN, previous):
        raise ValueError(f"Previous tag is not a version: {previous}")
    major, minor = re.match(r"v?([0-9]+)\.([0-9]+)", previous).groups()
    return f"{int(major)}.{int(minor) + 1}", True


def restore_published(root, published_ref):
    # Resolve Mike's latest alias, including older deployments using symlinks.
    versions = json.loads(git(root, "show", f"{published_ref}:versions.json"))
    latest = next((v["version"] for v in versions if "latest" in v["aliases"]), None)
    if latest:
        prefix = f"{latest}/schemas/"
        paths = git(root, "ls-tree", "-r", "--name-only", published_ref, prefix)
        for path in paths.splitlines():
            relative = path.removeprefix(prefix)
            if re.fullmatch(rf"{VERSION_PATTERN}/[^/]+\.schema\.json", relative):
                target = root / "docs/schemas" / relative
                target.parent.mkdir(parents=True, exist_ok=True)
                # Preserve previous releases byte for byte.
                target.write_bytes(subprocess.check_output(
                    ["git", "show", f"{published_ref}:{path}"], cwd=root
                ))


def render(root, version, development, output):
    """Render metadata without modifying the schema sources."""
    schema_path = f"develop/{version}" if development else version
    docs_version = "develop" if development else version
    output.mkdir(parents=True, exist_ok=True)
    for source in sorted((root / "src/tokamap/schemas").glob("*.schema.json")):
        schema = json.loads(source.read_bytes())
        schema["$id"] = f"{DOCS_URL}/{docs_version}/schemas/{schema_path}/{source.name}"
        description = re.sub(r" \(version [^)]+\)$", "", schema["description"])
        schema["description"] = f"{description} (version {version})"
        content = json.dumps(schema, indent=2, ensure_ascii=False) + "\n"
        (output / source.name).write_text(content)


def prepare(root, version=None, develop=False, docs=False, published_ref=None, output_dir=None):
    version, development = schema_version(root, version, develop)
    output = root / (output_dir or "build/schemas")
    render(root, version, development, output)
    if docs:
        if published_ref:
            restore_published(root, published_ref)
        schema_path = f"develop/{version}" if development else version
        docs_output = root / "docs/schemas" / schema_path
        docs_output.mkdir(parents=True, exist_ok=True)

        for schema in output.glob("*.schema.json"):
            (docs_output / schema.name).write_bytes(schema.read_bytes())
    return version


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", help="Exact release tag; otherwise infer from Git")
    parser.add_argument("--develop", action="store_true", help="Use the next minor version, even at a tagged commit")
    parser.add_argument("--docs", action="store_true", help="Also stage current and previous schemas for MkDocs")
    parser.add_argument("--published-ref", help="Git ref containing previously published docs")
    parser.add_argument("--output-dir", help="Rendered schema directory, relative to the repository root")
    args = parser.parse_args()
    print(prepare(Path(__file__).resolve().parents[1], **vars(args)))
