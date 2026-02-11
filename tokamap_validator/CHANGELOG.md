# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2024

### Added
- Initial release of TokaMap Validator
- JSON schema validation for TokaMap configurations
- Support for validating mappings.cfg.json files
- Support for validating globals.json files
- Support for validating mappings.json files
- Command-line interface with verbose output option
- Support for partitioned mapping structures
- Five mapping types: DIMENSION, VALUE, DATA_SOURCE, EXPR, CUSTOM
- Detailed error reporting for schema violations
- `--version` option to display the installed version of tokamap-validator
- Dynamic version resolution using `importlib.metadata`

### Features
- `tokamap-validator` command-line tool
- Recursive validation of partitioned structures
- Schema files bundled with package
- Python API for programmatic validation

[Unreleased]: https://github.com/ukaea/tokamap/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/ukaea/tokamap/releases/tag/v0.1.0
