import json

import jsonschema


class ValidationError:
    def __init__(self, path, data, errors=None):
        self.path = path
        self.data = data
        self.errors = errors or []

    def add_error(self, error_message):
        self.errors.append(error_message)


def validate_mappings(mappings):
    validation_errors = []
    for path, mapping in mappings.items():
        try:
            map_type = mapping["map_type"]
        except (TypeError, KeyError):
            map_type = "VALUE"

        match map_type:
            case "VALUE":
                validation_errors += validate_value_mapping(path, mapping, mappings)
            case "DIMENSION":
                validation_errors += validate_dimension_mapping(path, mapping, mappings)
            case "DATA_SOURCE":
                validation_errors += validate_data_source_mapping(
                    path, mapping, mappings
                )
            case "EXPR":
                validation_errors += validate_expr_mapping(path, mapping, mappings)
            case "CUSTOM":
                validation_errors += validate_custom_mapping(path, mapping, mappings)
            case _:
                validation_errors.append(
                    ValidationError(f"unknown map_type '{map_type}'", mapping)
                )

    return validation_errors


def validate_value_mapping(_path, _mapping, _mappings):
    validation_errors = []
    return validation_errors


def validate_dimension_mapping(path, mapping, mappings):
    validation_errors = []
    dim_probe = mapping["dim_probe"]
    if dim_probe not in mappings:
        validation_errors.append(
            ValidationError(
                path, mapping, [f"dim_probe '{dim_probe}' not found in mappings"]
            )
        )
    return validation_errors


def validate_data_source_mapping(_path, _mapping, _mappings):
    validation_errors = []
    return validation_errors


def validate_expr_mapping(path, mapping, mappings):
    validation_errors = []
    parameters = mapping["parameters"]
    for name, parameter in parameters.items():
        if parameter not in mappings:
            validation_errors.append(
                ValidationError(
                    path,
                    mapping,
                    [f"parameter ['{name}'] '{parameter}' not found in mappings"],
                )
            )
    return validation_errors


def validate_custom_mapping(path, mapping, mappings):
    validation_errors = []
    inputs = mapping["inputs"]
    for name, input in inputs.items():
        if input not in mappings:
            validation_errors.append(
                ValidationError(
                    path,
                    mapping,
                    [f"input ['{name}'] '{input}' not found in mappings"],
                )
            )
    return validation_errors


class Validator:
    def __init__(self, schema_file):
        with open(schema_file, "r") as f:
            self.schema = json.load(f)
            self._extract_map_types()

    def _extract_map_types(self):
        defs = self.schema["$defs"]
        self._valid_map_types = []
        for value in defs.values():
            if (
                "properties" in value
                and "map_type" in value["properties"]
                and "const" in value["properties"]["map_type"]
            ):
                self._valid_map_types.append(value["properties"]["map_type"]["const"])

    def validate(self, mappings_file):
        validation_errors = []

        with open(mappings_file, "r") as f:
            mappings = json.load(f)
            try:
                jsonschema.validate(mappings, self.schema)
            except jsonschema.ValidationError as e:
                error = ValidationError(e.absolute_path.pop(), e.instance)
                if "map_type" not in e.instance:
                    error.add_error("'map_type' not defined")
                else:
                    map_type = e.instance["map_type"]
                    if map_type not in self._valid_map_types:
                        error.add_error(f"invalid 'map_type': {map_type}")
                    else:
                        for context in e.context:
                            if "properties" in context.schema:
                                schema_map_type = context.schema["properties"][
                                    "map_type"
                                ]["const"]
                                if map_type == schema_map_type:
                                    error.add_error(f"{context.message}")
                validation_errors.append(error)

            if len(validation_errors) == 0:
                # if mappings validate against schema, run additional validation steps
                validation_errors += validate_mappings(mappings)

        return validation_errors
