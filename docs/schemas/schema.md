# Schema Structure

The TokaMap system consists of three main schema components:

- [Configuration Schema](config.md) (`mappings.cfg.schema.json`): Defines the top-level configuration structure
including.
- [Globals Schema](globals.md) (`globals.schema.json`): Defines global configuration settings, particularly data
source configurations with their associated arguments.
- [Mappings Schema](mappings.md) (`mappings.schema.json`): Defines the structure for mapping experimental data with
five main mapping types.

## Published schemas

Releases publish all three JSON schema files as documentation downloads and
GitHub release assets. Editors can use these URLs to retrieve schema definitions.
The schema version is the exact Git release tag. For example, release `0.3.2`
publishes:

- `https://ukaea.github.io/tokamap/latest/schemas/0.3.2/mappings.schema.json`
- `https://github.com/ukaea/tokamap/releases/download/0.3.2/mappings.schema.json`

Replace `mappings.schema.json` with `globals.schema.json` or
`mappings.cfg.schema.json` for the other schemas. The GitHub `/archive/refs/tags/`
URLs download source archives rather than individual release assets.

The docs build carries forward previously published schema versions so their
`latest/schemas/<version>/` links remain available after later releases.
Each release also has its own docs URL, with the release tag replacing `latest`.
Prereleases publish their own docs and release assets without moving `latest`.

During building, the shared schema renderer sets each schema's `$id` to its
release-specific documentation URL and includes the version in its description.
For example, the `$id` for the mappings schema in release `0.3.2` is
`https://ukaea.github.io/tokamap/0.3.2/schemas/0.3.2/mappings.schema.json`.
Docs downloads, release assets, and the PyPI package use the same rendering.
The JSON Schema dialect (`$schema`) and validation rules remain unchanged.

The checked-in schemas are valid JSON Schema documents with no `$id` and an
unversioned description. Normal package builds (`pip install .` or
`python -m build`) generate versioned schemas in the build output, without
modifying those source files. Source distributions contain the generated schemas
and retain their resolved schema version so rebuilding a wheel requires no Git
history. The renderer and build hooks are included in the source distribution.

Editable installs (`pip install -e .`) generate an ignored
`src/tokamap/_generated_schemas/` directory; `tokamap.schemas_dir()` returns it.
Reinstall after editing schema files or advancing to a different schema version
to refresh those generated files. A checkout without generated editable schemas
uses the anonymous source schemas.

Development builds use the next minor version after the nearest reachable Git
tag: `0.3.2` becomes `0.4`, and `v1.9.7` becomes `1.10`. These provisional schemas
are placed under `schemas/develop/<version>/` within the development docs. For
example:

`https://ukaea.github.io/tokamap/develop/schemas/develop/0.4/mappings.schema.json`

The final numeric directory lets consumers use the same version-suffix parsing
as for releases. Development URLs can change content as development progresses.
Release paths preserve the exact tag, including any `v` prefix or prerelease
suffix; consumers must allow those forms if the project uses them.

To include schemas in a local documentation build, run
`python scripts/prepare_schemas.py --docs` before `mkdocs build` or `mkdocs serve`.
The script detects a version tag on the current commit, otherwise it generates
the provisional next minor version. Use `--develop` to force development mode,
or `--version 0.3.2` to render a specific release. Git history and tags must be
available for automatic version detection.

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
