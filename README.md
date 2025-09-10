# TokaMap

TokaMap is a JSON schema-based framework for mapping and structuring experimental data. It provides a standardized way to define how experimental measurements, calculations, and data transformations should be organized and validated.

## Overview

TokaMap defines a set of JSON schemas that allow researchers and engineers to:

- **Map experimental data** using structured configuration files
- **Define data sources** with standardized parameters and arguments
- **Create expressions and calculations** from mapped data
- **Partition and group data** based on experimental attributes
- **Validate mappings** to ensure data consistency and correctness

## Architecture

The TokaMap system consists of three main schema components:

### 1. Mappings Schema (`mappings.schema.json`)

Defines the structure for mapping experimental data with five main mapping types:

- **DIMENSION**: Maps dimensional probe data
- **VALUE**: Maps static values (numbers, strings, arrays, objects)
- **DATA_SOURCE**: Maps external data sources with configurable arguments, offsets, scaling, and slicing
- **EXPR**: Maps mathematical expressions with parameters
- **CUSTOM**: Maps custom functions from external libraries

### 2. Globals Schema (`globals.schema.json`)

Defines global configuration settings, particularly data source configurations with their associated arguments.

### 3. Configuration Schema (`mappings.cfg.schema.json`)

Defines the top-level configuration structure including:

- **Metadata**: Experiment information, author, and version
- **Partitions**: Data partitioning rules with selectors (max_below, min_above, exact, closest)
- **Groups**: Array of group identifiers

## Schema Structure

### Mapping Types

Each mapping entry can be one of several types:

#### Dimension Mapping
```json
{
  "MAP_TYPE": "DIMENSION",
  "DIM_PROBE": "probe_identifier",
  "COMMENT": "Optional description"
}
```

#### Value Mapping
```json
{
  "MAP_TYPE": "VALUE",
  "VALUE": "any_value_type",
  "COMMENT": "Optional description"
}
```

#### Data Source Mapping
```json
{
  "MAP_TYPE": "DATA_SOURCE",
  "DATA_SOURCE": "source_name",
  "ARGS": {
    "arg1": "value1",
    "arg2": true
  },
  "OFFSET": 0,
  "SCALE": 1.0,
  "SLICE": "0:10",
  "COMMENT": "Optional description"
}
```

#### Expression Mapping
```json
{
  "MAP_TYPE": "EXPR",
  "EXPR": "mathematical_expression",
  "PARAMETERS": {
    "param1": "value1"
  },
  "COMMENT": "Optional description"
}
```

#### Custom Function Mapping
```json
{
  "MAP_TYPE": "CUSTOM",
  "LIBRARY": "library_name",
  "FUNCTION": "function_name",
  "INPUTS": {
    "input1": "value1"
  },
  "PARAMETERS": {},
  "COMMENT": "Optional description"
}
```

## Directory Structure

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

## Validation

TokaMap includes a Python validator tool to ensure your mapping files conform to the schemas.

### Installation

The validator is a Python package that can be installed using Poetry:

```bash
cd tokamap_validator
poetry install
```

### Usage

Validate a TokaMap mapping directory:

```bash
poetry run validator /path/to/mapping/directory
```

For verbose output:

```bash
poetry run validator -v /path/to/mapping/directory
```

The validator will:

1. Check that the configuration file (`mappings.cfg.json`) exists and is valid
2. Validate the top-level globals file
3. Validate each mapping group's globals and mappings files

### Validator Requirements

- Python >= 3.13
- jsonschema >= 4.25.0

## Getting Started

1. **Create a configuration file** (`mappings.cfg.json`) with your experiment metadata:

```json
{
  "metadata": {
    "experiment": "your_experiment_name",
    "author": "Your Name",
    "version": "1.0.0"
  },
  "partitions": [
    {
      "attribute": "time",
      "selector": "closest"
    }
  ],
  "groups": ["group1", "group2"]
}
```

2. **Define global settings** in `globals.json`:

```json
{
  "DATA_SOURCE_CONFIG": {
    "your_data_source": {
      "ARGS": {
        "connection_string": "your_connection"
      }
    }
  }
}
```

3. **Create mapping directories** for each group with their respective `mappings.json` and `globals.json` files.

4. **Validate your configuration** using the TokaMap validator.

## Schema Versioning

TokaMap schemas follow semantic versioning. The current version is 1.0 as indicated in the JSON Schema `$id` fields.

## Contributing

TokaMap is developed to support experimental data mapping workflows. The schemas are designed to be extensible while maintaining validation integrity.

## License

See the main project LICENSE file for license information.
