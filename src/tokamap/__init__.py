"TokaMap - A JSON schema-based framework for mapping and structuring experimental data"

__author__ = "Jonathan Hollocombe"
__email__ = "jonathan.hollocombe@ukaea.uk"

# Read version from package metadata (installed package)
# This ensures version is only defined in pyproject.toml
from sys import implementation

try:
    from importlib.metadata import PackageNotFoundError, version

    try:
        __version__ = version("tokamap")
    except PackageNotFoundError:
        # Package is not installed (e.g., running from source during development)
        __version__ = "0.0.0-dev"
except ImportError:
    # Fallback for Python < 3.8 (though we require 3.13+)
    __version__ = "0.0.0-dev"


def schemas_dir():
    """Return the directory containing the schemas"""
    from pathlib import Path

    return Path(__file__).parent / "schemas"
