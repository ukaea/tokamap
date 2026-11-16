import argparse
import json
import os
import sys
from pathlib import Path

from .. import __version__, schemas_dir
from .validate import Validator

# in a group passed to --expect-invalid, mappings with this prefix must fail validation
INVALID_PREFIX = "invalid_"


def validate_mapping(
    mapping_dir, mappings_validator, globals_validator, verbose, expect_invalid
):
    validate = True
    if verbose:
        print("Validating globals.json ...", end="", flush=True)
    globals_file = mapping_dir / "globals.json"
    errors = globals_validator.validate(globals_file)
    validate &= check_result(globals_file, errors, verbose)

    if verbose:
        print("Validating mappings.json ...", end="", flush=True)
    mappings_file = mapping_dir / "mappings.json"
    errors = mappings_validator.validate(mappings_file)
    if expect_invalid:
        validate &= check_expected_result(mappings_file, errors, verbose)
    else:
        validate &= check_result(mappings_file, errors, verbose)
    return validate


def validate_partitions(
    directory, partitions, mappings_validator, globals_validator, args, expect_invalid
):
    validate = True
    if len(partitions) == 0:
        if args.verbose:
            print(f"Validating mapping '{directory}'")
        validate &= validate_mapping(
            directory, mappings_validator, globals_validator, args.verbose, expect_invalid
        )
    else:
        partition = partitions[0]
        subdirs = [i for i in os.listdir(directory) if (directory / i).is_dir()]
        if not subdirs:
            print(
                f"Expected directories for partition '{partition}' in mapping '{directory}'.",
                file=sys.stderr,
            )
            sys.exit(1)

        for subdir in subdirs:
            validate &= validate_partitions(
                directory / subdir,
                partitions[1:],
                mappings_validator,
                globals_validator,
                args,
                expect_invalid,
            )

    return validate


def print_error(file_path, error, verbose):
    if verbose:
        print(file=sys.stderr)
    print(f"Failed to validate: {file_path} [{error.path}]", file=sys.stderr)
    if verbose:
        print(json.dumps(error.data, indent=2), file=sys.stderr)
    if len(error.errors) > 0:
        print("Validation errors:", file=sys.stderr)
        for error in error.errors:
            print(f"  - {error}", file=sys.stderr)


def check_result(file_path, errors, verbose):
    if len(errors) > 0:
        if verbose:
            print("error")
        for error in errors:
            print_error(file_path, error, verbose)
        return False
    elif verbose:
        print("ok")
    return True


def check_expected_result(file_path, errors, verbose):
    """
    Mappings named invalid_* must fail validation, every other mapping must pass
    """
    with open(file_path, "r") as f:
        names = json.load(f).keys()

    expected = {name for name in names if name.startswith(INVALID_PREFIX)}
    failed = {error.path for error in errors}

    missed = sorted(expected - failed)
    unexpected = [error for error in errors if error.path not in expected]

    if not missed and not unexpected:
        if verbose:
            print(f"ok ({len(expected)} expected failures)")
        return True

    if verbose:
        print("error")
    for name in missed:
        print(
            f"Expected to fail validation but passed: {file_path} [{name}]",
            file=sys.stderr,
        )
    for error in unexpected:
        print_error(file_path, error, verbose)
    return False


def run():
    parser = argparse.ArgumentParser(description="Validate a TokaMap mapping directory")
    parser.add_argument(
        "--version", action="version", version=f"tokamap-validator {__version__}"
    )
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Enable verbose output"
    )
    parser.add_argument(
        "--expect-invalid",
        action="append",
        default=[],
        metavar="GROUP",
        help=(
            f"In this group, mappings named '{INVALID_PREFIX}*' must fail validation and "
            "all others must pass (can be given more than once)"
        ),
    )
    parser.add_argument("directory", help="TokaMap mapping directory to validate")
    args = parser.parse_args()

    root = schemas_dir()
    config_validator = Validator(os.path.join(root, "mappings.cfg.schema.json"))
    mappings_validator = Validator(os.path.join(root, "mappings.schema.json"))
    globals_validator = Validator(os.path.join(root, "globals.schema.json"))

    root = Path(args.directory)
    if not root.is_dir():
        print(f"Argument '{args.directory}' is not a directory.", file=sys.stderr)
        return 1

    config_file = root / "mappings.cfg.json"
    if not config_file.exists():
        print(f"Configuration file '{config_file}' does not exist.", file=sys.stderr)
        return 1

    validate = True

    if args.verbose:
        print("Validating configuation file ... ", end="")
    errors = config_validator.validate(config_file)
    validate &= check_result(config_file, errors, args.verbose)

    if args.verbose:
        print("Reading configuration file 'mappings.cfg.json'")

    with open(config_file, "r") as f:
        config = json.load(f)

    toplevel_globals = root / "globals.json"
    if not toplevel_globals.exists():
        print(f"Root level globals file '{toplevel_globals}' does not exist.", file=sys.stderr)
        return 1

    if args.verbose:
        print("Validating top-level globals ... ", end="")
    errors = globals_validator.validate(toplevel_globals)
    validate &= check_result(toplevel_globals, errors, args.verbose)

    partitions = config["partitions"]
    mappings = config["groups"]

    # catch typos rather than silently validating the group normally
    unknown = [group for group in args.expect_invalid if group not in mappings]
    if unknown:
        print(f"--expect-invalid groups not found in '{config_file}': {unknown}", file=sys.stderr)
        return 1

    if args.verbose:
        print(f"Mappings found: {mappings}")

    for mapping in mappings:
        mapping_dir = root / mapping
        if not mapping_dir.exists():
            print(f"Mapping '{mapping}' does not exist.", file=sys.stderr)
            return 1

        validate &= validate_partitions(
            mapping_dir,
            partitions,
            mappings_validator,
            globals_validator,
            args,
            mapping in args.expect_invalid,
        )

    if validate:
        print("Validation completed successfully.")
        return 0

    print("Validation failed.", file=sys.stderr)
    return 1
