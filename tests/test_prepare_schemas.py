import json
from pathlib import Path
import subprocess
import tempfile
import unittest

from scripts.prepare_schemas import prepare, schema_version


class PrepareSchemasTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.root = Path(self.directory.name)
        self.source = self.root / "src/tokamap/schemas"
        self.source.mkdir(parents=True)
        self.original = {
            "$schema": "https://json-schema.org/draft/2020-12/schema",
            "description": "Original description",
            "type": "object",
            "properties": {"example": {"type": "string"}},
        }
        for name in ("mappings", "globals", "mappings.cfg"):
            (self.source / f"{name}.schema.json").write_text(json.dumps(self.original))
        self.git("init", "-q")
        self.git("config", "user.name", "Test")
        self.git("config", "user.email", "test@example.com")
        self.git("add", ".")
        self.git("commit", "-qm", "Initial schemas")
        self.git("tag", "0.3.2")

    def git(self, *args):
        return subprocess.check_output(["git", *args], cwd=self.root).decode().strip()

    def test_exact_tag_and_explicit_release_versions(self):
        self.assertEqual(schema_version(self.root), ("0.3.2", False))
        for tag in ("0.4", "v0.4.0", "0.4.0-rc.1", "0.4.0+build.1"):
            with self.subTest(tag=tag):
                self.assertEqual(schema_version(self.root, tag), (tag, False))

    def test_development_uses_next_minor_even_at_tagged_commit(self):
        self.assertEqual(schema_version(self.root, develop=True), ("0.4", True))
        self.git("commit", "--allow-empty", "-qm", "Development change")
        self.assertEqual(schema_version(self.root), ("0.4", True))
        self.git("tag", "v1.9.7")
        self.assertEqual(schema_version(self.root, develop=True), ("1.10", True))

    def test_release_renders_same_schemas_for_docs_assets_and_package(self):
        prepare(self.root, docs=True)
        for source in self.source.iterdir():
            output = self.root / "build/schemas" / source.name
            docs = self.root / "docs/schemas/0.3.2" / source.name
            self.assertEqual(output.read_bytes(), docs.read_bytes())
            schema = json.loads(output.read_bytes())
            self.assertEqual(schema["$id"],
                             f"https://ukaea.github.io/tokamap/0.3.2/schemas/0.3.2/{source.name}")
            self.assertIn("0.3.2", schema["description"])
            for key in self.original.keys() - {"$id", "description"}:
                self.assertEqual(schema[key], self.original[key])
            self.assertEqual(json.loads(source.read_bytes()), self.original)

        prepare(self.root, output_dir="build/package-schemas")
        for source in self.source.iterdir():
            self.assertEqual((self.root / "build/package-schemas" / source.name).read_bytes(),
                             (self.root / "build/schemas" / source.name).read_bytes())

    def test_development_is_staged_beneath_develop_and_numeric_version(self):
        prepare(self.root, develop=True, docs=True)
        path = self.root / "docs/schemas/develop/0.4/mappings.schema.json"
        schema = json.loads(path.read_bytes())
        self.assertEqual(schema["$id"],
                         "https://ukaea.github.io/tokamap/develop/schemas/develop/0.4/mappings.schema.json")
        self.assertIn("0.4", schema["description"])

    def test_keeps_old_schema_versions_from_latest_alias_target(self):
        # Model a previous Mike deployment, including its default symlink alias.
        old_schemas = {}
        for version in ("0.3.1", "v0.3.0-rc.1", "0.2"):
            old = self.root / f"0.3.2/schemas/{version}/mappings.schema.json"
            old.parent.mkdir(parents=True)
            old.write_text(json.dumps({"description": f"previous schema {version}"}))
            old_schemas[version] = old.read_bytes()
        (self.root / "latest").symlink_to("0.3.2")
        development = self.root / "0.3.2/schemas/develop/0.4/mappings.schema.json"
        development.parent.mkdir(parents=True)
        development.write_text('{"description": "Unreleased schema"}')
        (self.root / "versions.json").write_text(json.dumps([
            {"version": "0.3.2", "aliases": ["latest"]}
        ]))
        self.git("add", ".")
        self.git("commit", "-qm", "Published docs")

        prepare(self.root, version="0.4.0", docs=True, published_ref="HEAD")

        for version, content in old_schemas.items():
            restored = self.root / f"docs/schemas/{version}/mappings.schema.json"
            self.assertEqual(restored.read_bytes(), content)
        self.assertTrue((self.root / "docs/schemas/0.4.0/globals.schema.json").is_file())
        self.assertFalse((self.root / "docs/schemas/develop").exists())

    def test_rejects_invalid_or_conflicting_version_arguments(self):
        for version in ("../invalid", "0.3.2/extra", "develop", "0"):
            with self.subTest(version=version), self.assertRaisesRegex(ValueError, "version tag"):
                prepare(self.root, version=version)
        with self.assertRaisesRegex(ValueError, "either"):
            prepare(self.root, version="0.3.2", develop=True)


if __name__ == "__main__":
    unittest.main()
