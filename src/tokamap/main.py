import argparse

from . import __version__, schemas_dir


def run():
    parser = argparse.ArgumentParser(description="Validate a TokaMap mapping directory")
    parser.add_argument(
        "--version", action="version", version=f"tokamap-validator {__version__}"
    )
    parser.add_argument(
        "--schemas-dir",
        action="store_true",
        help="Print the path to the schemas directory",
    )

    args = parser.parse_args()

    if args.schemas_dir:
        print(schemas_dir())
