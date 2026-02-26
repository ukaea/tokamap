# TokaMap

TokaMap is a JSON schema-based framework for mapping and structuring experimental data. It provides a standardized way to define how experimental measurements, calculations, and data transformations should be organized and validated.

The Python package contains the following components:

- [tokamap](python.md): A module that provides details about the install, such as the schemas directory.
- [tokamap_validator](validator.md): A validation tool for ensuring consistency and correctness of mappings.

## Overview

TokaMap defines a set of JSON schemas that allow researchers and engineers to:

- **Map experimental data** using structured configuration files
- **Define data sources** with standardized parameters and arguments
- **Create expressions and calculations** from mapped data
- **Partition and group data** based on experimental attributes
- **Validate mappings** to ensure data consistency and correctness

Details of the schema structure can be found in the [schemas](schemas/schema.md) section.

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
      "attribute": "experiment_number",
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

## Contributing

We appreciate any suggestions and contributions given using the GitHub issues tracker.

## License

See the main project LICENSE file for license information.
