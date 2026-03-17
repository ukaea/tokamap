# TokaMap

TokaMap is a JSON schema-based framework for mapping experimental data. It provides a standardized way to define how experimental measurements, calculations, and data transformations should be organized and validated.

## Overview

TokaMap defines a set of JSON schemas that allow researchers and engineers to:

- **Map experimental data** using structured configuration files
- **Define data sources** with standardized parameters and arguments
- **Create expressions and calculations** from mapped data
- **Partition and group data** based on experimental attributes
- **Validate mappings** to ensure data consistency and correctness

Details of the schema structure can be found in the [schemas](schemas/schema.md) section.

## Python library

A Python package is available that installs the schemas alongside a few usage tools:

- `tokamap`: A CLI that provides details about the install, such as the schemas directory.
- `tokamap-validator`: A validation tool for ensuring consistency and correctness of mappings.

More details about the provided library and tools can be found in the [python](python.md) section.

## Getting Started

1. **Create a configuration file** (`mappings.cfg.json`) with your experiment metadata:

```json
{
  "metadata": {
    "experiment": "experiment_name",
    "author": "Your Name",
    "version": "1.0.0"
  },
  "partitions": [
    {
      "attribute": "experiment_number",
      "selector": "closest"
    }
  ],
  "groups": ["group1", "group2"]
}
```

More details can be found in the [config](schemas/config.md) section.

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

More details can be found in the [globals](schemas/globals.md) section.

3. **Create mapping directories** for each group and partition, containing `mappings.json` and `globals.json` files.

```
mappings/
└── experiment_name/
    ├── mappings.cfg.json     # Experiment configuration and metadata
    ├── globals.json          # Top-level globals
    ├── group1/
    │   └── 0/                # Partition value
    │       ├── globals.json  # Partition globals
    │       └── mappings.json # Actual mappings
    └── group2/
        └── 0/                # Partition value
            ├── globals.json  # Partition globals
            └── mappings.json # Actual mappings
```

I.e. for the `mappings.json`:

```json
{
  "value_mapping": "value",
  "data_source_mapping": {
    "map_type": "DATA_SOURCE",
    "data_source": "MyDataSource",
    "args": {
      "signal": "coils"
    }
  }
}
```

See [globals](schemas/globals.md) and [mappings](schemas/mappings.md) sections for details.

4. **Validate your configuration** using the TokaMap validator.

You can validate your mappings using the `tokamap-validator` by running (assuming the directory structure above):

```bash
tokamap-validator mappings/
```

This will loop over the `mappings` directory looking for directories containing a `mappings.cfg.json` and validating each directory found.

## Contributing

We appreciate any suggestions and contributions given using the GitHub issues tracker.

## License

See the main project LICENSE file for license information.
