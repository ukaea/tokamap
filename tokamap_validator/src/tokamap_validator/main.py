import argparse
import json
import os
from pathlib import Path

from . import __version__
from .validate import Validator


def validate_mapping(mapping_dir, mappings_validator, globals_validator, verbose):
    globals_file = mapping_dir / "globals.json"
    globals_validator.validate(globals_file)

    mappings_file = mapping_dir / "mappings.json"
    mappings_validator.validate(mappings_file, upper_case=True)


def validate_partitions(
    directory, partitions, mappings_validator, globals_validator, args
):
    if len(partitions) == 0:
        if args.verbose:
            print(f"Validating mapping '{directory}' ... ", end="")
        validate_mapping(directory, mappings_validator, globals_validator, args.verbose)
        if args.verbose:
            print("ok")
    else:
        partition = partitions[0]
        subdirs = [i for i in os.listdir(directory) if (directory / i).is_dir()]
        if not subdirs:
            print(
                f"Expected directories for partition '{partition}' in mapping '{directory}'."
            )
            exit(1)

        for subdir in subdirs:
            validate_partitions(
                directory / subdir,
                partitions[1:],
                mappings_validator,
                globals_validator,
                args,
            )


def run():
    parser = argparse.ArgumentParser(description="Validate a TokaMap mapping directory")
    parser.add_argument(
        "--version", action="version", version=f"tokamap-validator {__version__}"
    )
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Enable verbose output"
    )
    parser.add_argument("directory", help="TokaMap mapping directory to validate")
    args = parser.parse_args()

    schemas_dir = os.path.join(os.path.dirname(__file__), "schemas")
    config_validator = Validator(os.path.join(schemas_dir, "mappings.cfg.schema.json"))
    mappings_validator = Validator(os.path.join(schemas_dir, "mappings.schema.json"))
    globals_validator = Validator(os.path.join(schemas_dir, "globals.schema.json"))

    root = Path(args.directory)
    if not root.is_dir():
        print(f"Argument '{args.directory}' is not a directory.")
        return

    config_file = root / "mappings.cfg.json"
    if not config_file.exists():
        print(f"Configuration file '{config_file}' does not exist.")
        return

    if args.verbose:
        print("Validating configuation file ... ", end="")
    config_validator.validate(config_file)
    if args.verbose:
        print("ok")

    if args.verbose:
        print("Reading configuration file 'mappings.cfg.json'")

    with open(config_file, "r") as f:
        config = json.load(f)

    toplevel_globals = root / "globals.json"
    if not toplevel_globals.exists():
        print(f"Root level globals file '{toplevel_globals}' does not exist.")
        return

    if args.verbose:
        print("Validating top-level globals ... ", end="")
    globals_validator.validate(toplevel_globals)
    if args.verbose:
        print("ok")

    partitions = config["partitions"]
    mappings = config["groups"]

    if args.verbose:
        print(f"Mappings found: {mappings}")

    for mapping in mappings:
        mapping_dir = root / mapping
        if not mapping_dir.exists():
            print(f"Mapping '{mapping}' does not exist.")
            exit(1)

        validate_partitions(
            mapping_dir, partitions, mappings_validator, globals_validator, args
        )

    print("Validation completed successfully.")
