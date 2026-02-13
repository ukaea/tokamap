import json
import jsonschema

class Validator:
    def __init__(self, schema_file):
        with open(schema_file, 'r') as f:
            self.schema = json.load(f)

    def validate(self, mappings_file, upper_case=False):
        with open(mappings_file, 'r') as f:
            mappings = json.load(f)
            if upper_case:
                for key, value in mappings.items():
                    if isinstance(value, dict):
                        mappings[key] = {k.upper(): v for k, v in value.items()}
            jsonschema.validate(mappings, self.schema)
