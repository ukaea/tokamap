## Schema Structure

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

### Mapping Types

Each mapping entry can be one of several types:

- Dimension: Extract the dimensionality of another mapping
- Value: A static value defined in the mapping files
- Data Source: Call an external data source to return the data
- Expression: Evaluate a mathematical expression with inputs from other mappings
- Custom Function: Call a custom function defined in a library

Details of these mappings can be found [here](mapping_types.md).

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
