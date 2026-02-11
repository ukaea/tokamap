# TokaMap Validator

A Python validator tool for TokaMap experimental data mapping configurations. This package validates TokaMap mapping files against JSON schemas to ensure data consistency and correctness.

## Overview

TokaMap is a JSON schema-based framework for mapping and structuring experimental data. The TokaMap Validator ensures that your mapping configurations conform to the TokaMap schema specifications.

## Features

- ✅ Validates TokaMap configuration files (`mappings.cfg.json`)
- ✅ Validates global settings files (`globals.json`)
- ✅ Validates mapping files (`mappings.json`)
- ✅ Supports partitioned mapping structures
- ✅ Provides detailed error messages for schema violations
- ✅ Command-line interface with verbose output option

## Installation

### From PyPI

```bash
pip install tokamap-validator
```

### From Source

```bash
git clone https://github.com/ukaea/tokamap.git
cd tokamap/tokamap_validator
pip install .
```

## Requirements

- Python >= 3.13
- jsonschema >= 4.25.0

## Usage

### Command Line

Validate a TokaMap mapping directory:

```bash
tokamap-validator /path/to/mapping/directory
```

For verbose output with detailed validation progress:

```bash
tokamap-validator -v /path/to/mapping/directory
```

Check the installed version:

```bash
tokamap-validator --version
```

### Python API

You can also use the validator programmatically:

```python
from tokamap_validator.validate import Validator

# Create a validator with a schema file
validator = Validator('/path/to/schema.json')

# Validate a JSON file
validator.validate('/path/to/file.json')
```

## TokaMap Directory Structure

A typical TokaMap project follows this structure:

```
mapping_root/
├── mappings.cfg.json          # Main configuration file
├── globals.json               # Global settings
├── mapping_group_1/
│   ├── globals.json          # Group-specific globals
│   └── mappings.json         # Group mappings
└── mapping_group_2/
    ├── globals.json
    └── mappings.json
```

## Validation Process

The validator performs the following checks:

1. **Configuration Validation**: Verifies that `mappings.cfg.json` exists and conforms to the configuration schema
2. **Top-level Globals Validation**: Validates the root-level `globals.json` file
3. **Group Validation**: For each mapping group defined in the configuration:
   - Validates the group's `globals.json` file
   - Validates the group's `mappings.json` file
4. **Partition Support**: Handles partitioned mapping structures recursively

## Mapping Types

TokaMap supports five mapping types:

- **DIMENSION**: Maps dimensional probe data
- **VALUE**: Maps static values (numbers, strings, arrays, objects)
- **DATA_SOURCE**: Maps external data sources with configurable parameters
- **EXPR**: Maps mathematical expressions with parameters
- **CUSTOM**: Maps custom functions from external libraries

## Example Configuration

### mappings.cfg.json

```json
{
  "metadata": {
    "experiment": "my_experiment",
    "author": "Your Name",
    "version": "1.0.0"
  },
  "partitions": [
    {
      "attribute": "time",
      "selector": "closest"
    }
  ],
  "groups": ["diagnostics", "analysis"]
}
```

### globals.json

```json
{
  "DATA_SOURCE_CONFIG": {
    "my_data_source": {
      "ARGS": {
        "connection_string": "tcp://localhost:8080"
      }
    }
  }
}
```

### mappings.json

```json
{
  "temperature": {
    "MAP_TYPE": "DATA_SOURCE",
    "DATA_SOURCE": "my_data_source",
    "ARGS": {
      "signal": "TEMP_01"
    },
    "SCALE": 1.0,
    "COMMENT": "Temperature measurement from sensor 01"
  },
  "calculated_value": {
    "MAP_TYPE": "EXPR",
    "EXPR": "a * b + c",
    "PARAMETERS": {
      "a": 2.5,
      "b": "temperature",
      "c": 10
    }
  }
}
```

## Error Messages

The validator provides clear error messages when validation fails:

```
Validation error in file: /path/to/mappings.json
'MAP_TYPE' is a required property
```

## Testing

### Verify Dynamic Versioning

Run the comprehensive test suite to verify dynamic versioning is working correctly:

```bash
python test_dynamic_versioning.py
```

This tests that:
- Version is read from `pyproject.toml`
- Package metadata is correct
- `__version__` variable is set correctly
- CLI `--version` displays the correct version
- Development mode fallback works

All tests should pass if the package is installed correctly.

## Contributing

Contributions are welcome! Please visit the [GitHub repository](https://github.com/ukaea/tokamap) to:

- Report bugs
- Request features
- Submit pull requests

## License

This project is licensed under the MIT License - see the [LICENSE](https://github.com/ukaea/tokamap/blob/main/LICENSE) file for details.

## Links

- [Homepage](https://github.com/ukaea/tokamap)
- [Issue Tracker](https://github.com/ukaea/tokamap/issues)
- [Documentation](https://github.com/ukaea/tokamap#readme)

## Authors

- Jonathan Hollocombe (jonathan.hollocombe@ukaea.uk)

## Acknowledgments

Developed by UKAEA to support experimental data mapping workflows.