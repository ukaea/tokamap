# Schema Structure

The TokaMap system consists of three main schema components:

- [Configuration Schema](config.md) (`mappings.cfg.schema.json`): Defines the top-level configuration structure
including.
- [Globals Schema](globals.md) (`globals.schema.json`): Defines global configuration settings, particularly data
source configurations with their associated arguments.
- [Mappings Schema](mappings.md) (`mappings.schema.json`): Defines the structure for mapping experimental data with
five main mapping types.

## Directory Structure

A typical TokaMap project follows this structure:

```
mapping_root/
├── mappings.cfg.json          # Main configuration file
├── globals.json               # Global settings
├── mapping_group_1/
│   ├── globals.json           # Group-specific globals
│   └── mappings.json          # Group mappings
└── mapping_group_2/
    ├── globals.json
    └── mappings.json
```
