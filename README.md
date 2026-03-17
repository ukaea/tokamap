# TokaMap

TokaMap is a JSON schema-based framework for mapping and structuring experimental data. It provides a standardized way
to define how experimental measurements, calculations, and data transformations should be organized and validated.

The Python package contains the following components:

- **tokamap**: A module that provides details about the install, such as the schemas directory
- **tokamap.validator**: The validation module for ensuring data consistency and correctness

And the following CLI tools:

- **tokamap**: Command-line interface for querying details about the install, such as the schemas directory
- **tokamap-validator**: Command-line interface for validating TokaMap mappings

## Overview

TokaMap defines a set of JSON schemas that allow researchers and engineers to:

- **Map experimental data** using structured configuration files
- **Define data sources** with standardized parameters and arguments
- **Create expressions and calculations** from mapped data
- **Partition and group data** based on experimental attributes
- **Validate mappings** to ensure data consistency and correctness

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

print(schemas_dir())
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

The validator will:

1. Check that the configuration file (`mappings.cfg.json`) exists and is valid
2. Validate the top-level globals file
3. Validate each mapping group's globals and mappings files

### Validator Requirements

- Python >= 3.13
- jsonschema >= 4.25.0

## Architecture

The TokaMap system consists of three main schema components:

### 1. Configuration Schema (`mappings.cfg.schema.json`)

Defines the top-level configuration structure including:

- **Metadata**: Experiment information, author, and version
- **Partitions**: Data partitioning rules with selectors (max_below, min_above, exact, closest)
- **Groups**: Array of group identifiers

### 2. Globals Schema (`globals.schema.json`)

Defines global configuration settings, particularly data source configurations with their associated arguments.

### 3. Mappings Schema (`mappings.schema.json`)

Defines the structure for mapping experimental data with five main mapping types:

- **DIMENSION**: Maps dimensional probe data
- **VALUE**: Maps static values (numbers, strings, arrays, objects)
- **DATA_SOURCE**: Maps external data sources with configurable arguments, offsets, scaling, and slicing
- **EXPR**: Maps mathematical expressions with parameters
- **CUSTOM**: Maps custom functions from external libraries

For more information see the [docs](https://ukaea.github.io/tokamap).

## Contributing

TokaMap is developed to support experimental data mapping workflows. The schemas are designed to be extensible while maintaining validation integrity.

## License

See the main project LICENSE file for license information.
