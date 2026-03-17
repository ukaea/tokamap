# Mapping globals

The globals files provide a location to store 'global' data that can be used within your mappings. These values can be
any valid JSON data with the ability to access into arrays/object using the templating syntax.

The only restricted key is `data_source_config` which is used to provide configuration for data sources which are used
for every use of this data source. This allows for commonly set values to be set in the globals to avoid repetition.

The globals are cascading, with `globals.json` in subdirectories overriding the values provided in higher level
directories.

## Data source config

```json
"data_source_config": {
  "DataSource1": {
    "args": {
      "ArgumentName": "DefaultValue",
      ...
    }
  },
  "DataSource2": {
    ...
  }
}
```

If the `data_source_config` value is given it must be a dictionary with the keys being the name of the data source being
configured. Each data source is a dictionary containing the following configuration:

- **args**: A dictionary of arguments to pass into the data source.

## Value rendering

The values within the globals will be rendered using the templating syntax - they can contain references to other
globals and mapping selector attributes.

This is also true for the `args` given for the `data_source_config`.

## Examples

An example of a `globals.json`:

```json
{
  "data_source_config": {
    "JSON": {
      "args": {
        "file_name": "data_{{ shot }}.json"
      }
    }
  },
  "array_global": [1, 2, 3, 4, 5],
  "string_global": "string_value",
  "integer_global": 42,
  "float_global": 3.14,
  "object_global": {
    "name": "value"
  },
  "rendered_global": "{{ string_global }}_postfix"
}
```

The `shot` used in the `JSON.args.file_name` argument is using a mapping selector, so this relies on these mappings
being used with a given shot value.
