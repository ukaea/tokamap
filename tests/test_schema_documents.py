import json
from pathlib import Path
import unittest

from jsonschema import Draft202012Validator


SCHEMAS = Path(__file__).resolve().parents[1] / "src/tokamap/schemas"


class SchemaDocumentTests(unittest.TestCase):
    def test_source_schemas_are_anonymous_and_accept_optional_version_urls(self):
        for name, document in (
            ("mappings", {}),
            ("globals", {}),
            ("mappings.cfg", {"metadata": {}, "partitions": [], "groups": []}),
        ):
            with self.subTest(schema=name):
                schema = json.loads((SCHEMAS / f"{name}.schema.json").read_text())
                Draft202012Validator.check_schema(schema)
                self.assertNotIn("$id", schema)
                self.assertNotIn("1.1", schema["description"])
                validator = Draft202012Validator(schema)
                validator.validate(document)
                for version in ("0.4", "0.3.2", "v0.3.2", "0.4.0-rc.1", "0.4.0+build.1"):
                    for prefix in (
                        "https://ukaea.github.io/tokamap/latest/schemas",
                        "https://github.com/ukaea/tokamap/releases/download",
                        "https://example.org/schemas", "../../schemas",
                    ):
                        validator.validate({
                            **document, "$schema": f"{prefix}/{version}/{name}.schema.json",
                        })
                validator.validate({
                    **document,
                    "$schema": f"https://ukaea.github.io/tokamap/develop/schemas/develop/0.4/{name}.schema.json",
                })
                for invalid in (
                    None, 42, "https://json-schema.org/draft/2020-12/schema",
                    "https://example.org/0.3.2/wrong.schema.json",
                    f"https://example.org/invalid/{name}.schema.json",
                    f"https://example.org/0.3.2/{name}.schema.json/extra",
                ):
                    self.assertFalse(validator.is_valid({**document, "$schema": invalid}), invalid)
