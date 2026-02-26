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
