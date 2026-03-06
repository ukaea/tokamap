# Python library

The Python package contains the following components:

- `tokamap`: The top level module that provides details about the install as well as a simple CLI.
- `tokamap.validator`: Validation module for ensuring consistency and correctness of mappings.

The schemas as also installed as package data, with the location available via the Python library or a simple CLI tool.

## Installation

### From PyPI

```bash
pip install tokamap
```

### From Source

```bash
git clone https://github.com/ukaea/tokamap.git
pip install .
```

## TokaMap CLI

The TokaMap library comes with a command-line tool that provides functionality to query details about the schema install,
such as the location of the schemas.

Finding the schemas directory:

```bash
tokamap --schemas-dir
```

Checking the installed version:

```bash
tokamap --version
```

## TokaMap library

Finding the schemas directory:

```python
from tokamap import schemas_dir

print(schemas_dir)
```

Checking the installed version:

```python
from tokamap import __version__

print(__version__)
```

## Validation

TokaMap includes a Python validator tool to ensure your mapping files conform to the schemas.

### Usage

Validate a TokaMap mapping directory:

```bash
tokamap-validator /path/to/mapping/directory
```

For verbose output:

```bash
tokamap-validator -v /path/to/mapping/directory
```

Check the installed version:

```bash
tokamap-validator --version
```

The validator will:

1. Check that the configuration file (`mappings.cfg.json`) exists and is valid
2. Validate the top-level globals file
3. Validate each mapping group's globals and mappings files
