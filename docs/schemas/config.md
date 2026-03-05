# Mapping config file

The mappings (`mappings.cfg.json`) is required in the top level mapping directory.

This file provides metadata for the mappings and details of how the mappings are structured in the subdirectories.

## Structure

```json
{
  "metadata": {
    "experiment": "ExperimentName",
    "author": "Mapping Author",
    "version": "1.0.0"
  },
  "groups": [],
  "partitions": []
}
```

### Metadata

The required metadata fields are:

- **experiment**: The name of the experiment. This is the key used when selecting the mappings.
- **author**: The author(s) of the mappings.
- **version**: The version of the mappings. This mapping must be of the form `N.N.N` and should follow the
[semantic](https://semver.org/) versioning scheme.

The experiment & version pair must be unique amongst the located mappings.

### Groups

Inside the mapping directory each mapping is divided into 'groups'. For IMAS these groups correspond to the IMAS IDS
structures (magnetics, equilibrium, etc.) but should be used for any logical structuring of data for your experiment.

For each group specified there should be a subdirectory containing `globals.json` and `mappings.json` files.

### Partitions

Optionally you can partition you mappings below the 'group' subdirectory. This provides a method that can be used to
select the correct mapping files depending on given mapping attribute.

For each given selector (in the specified sequence) there should be nested directory inside the 'group' subdirectory
with directory name specifying the partition value, i.e. `group1/0/` for partition value `0` inside group `group1`.
If no partitions are given then the `globals.json` and `mappings.json` files should live directly under the 'group'
subdirectory.

For example if your mappings might change based on an 'experiment number' you might configure the following partitions
to allow selecting the highest directory name that is below the given experiment number:

```
"partitions": [
  { "attribute": "experiment_number", "selector": "MAX_BELOW" }
]
```

The different `selector`s that can be specified are given below.

#### Partition selectors

- **MAX_BELOW**: Select the highest possible directory value that is **below** the given value.
- **MIN_ABOVE**: Select the lowest possible directory value that is **above** the given value.
- **EXACT**: Select the partition directory that exactly matches the given value, or error if no partition found.
- **CLOSEST**: Select the partition directory that is closest to the given value.

For numeric selectors (`MAX_BELOW`, `MIN_ABOVE` and `CLOSEST`) the partition directory names must be convertable into
integers.
