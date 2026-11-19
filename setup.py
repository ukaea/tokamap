"""Setuptools hooks for generated schema metadata."""

import json
import os
from pathlib import Path
import sys

from setuptools import setup
from setuptools.command.build_py import build_py
from setuptools.command.sdist import sdist

ROOT = Path(__file__).resolve().parent
sys.path.insert(0, str(ROOT))
from scripts.prepare_schemas import render, schema_version


def build_schema_version():
    if (ROOT / ".schema-version.json").is_file():
        return schema_version(ROOT)
    return schema_version(
        ROOT,
        version=os.environ.get("TOKAMAP_SCHEMA_VERSION") or None,
        develop=os.environ.get("TOKAMAP_SCHEMA_DEVELOP") == "1",
    )


class BuildSchemas(build_py):
    def run(self):
        super().run()
        version, development = build_schema_version()
        output = (
            ROOT / "src/tokamap/_generated_schemas" if self.editable_mode
            else Path(self.build_lib) / "tokamap/schemas"
        )
        render(ROOT, version, development, output)

    def get_output_mapping(self):
        mapping = super().get_output_mapping()
        if self.editable_mode:
            for source in (ROOT / "src/tokamap/schemas").glob("*.schema.json"):
                target = str(Path(self.build_lib) / "tokamap/schemas" / source.name)
                mapping[target] = str(ROOT / "src/tokamap/_generated_schemas" / source.name)
        return mapping


class SourceSchemas(sdist):
    def make_release_tree(self, base_dir, files):
        super().make_release_tree(base_dir, files)
        version, development = build_schema_version()
        destination = Path(base_dir)
        (destination / ".schema-version.json").write_text(json.dumps({
            "version": version, "development": development,
        }) + "\n")
        output = destination / "src/tokamap/schemas"
        # Setuptools can hard-link source files into the release tree.
        # Break those links before rendering so the checkout stays unchanged.
        for schema in output.glob("*.schema.json"):
            schema.unlink()
        render(ROOT, version, development, output)


setup(cmdclass={"build_py": BuildSchemas, "sdist": SourceSchemas})
