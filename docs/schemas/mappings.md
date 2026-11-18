# Mappings

The mappings file contains a dictionary of `mapping_key`s to mapping values, where each one is defined as:

```json
{
  "mapping_key": {
    "map_type": "",
    ...
  },
  ...
}
```

Each mapping must contain a `map_type` with one of the following values:

- **DIMENSION**: Extract the dimensionality of another mapping
- **VALUE**: A static value defined in the mapping files
- **DATA_SOURCE**: Call an external data source to return the data
- **EXPR**: Evaluate a mathematical expression with inputs from other mappings
- **CUSTOM**: Call a custom function defined in a library

More details for each type, along with the required extra values, are given below.

## Syntactic sugar

To reduce boilerplate for `VALUE` mappings, these can be defined as:

```json
{
  "string_mapping": "string",
  "integer_mapping": 42,
  "float_mapping": 3.14,
  ...
}
```

These mappings are equivalent to the verbose syntax, i.e.

```json
{
  "string_mapping": "string",
  ...
}

<=>

{
  "string_mapping": {
    "map_type": "VALUE",
    "value": "string"
  },
  ...
}
```

## Mapping types

Each map type has required extra parameters.

The `comment` parameter can be provided for any map type but is always optional. This parameter allow for documentation
of the mapping as JSON does not support comments.

### Value Mapping

Return a static value defined in the mapping files.

```json
{
  "map_type": "VALUE",
  "value": "any_value_type",
  "comment": "Optional description"
}
```

**Parameters**:

- `value`: The static value to return.

### Dimension Mapping

Extract the dimensionality of another mapping. Currently only the length of the first dimension can be returned.

```json
{
  "map_type": "DIMENSION",
  "dim_probe": "probe_identifier",
  "comment": "Optional description"
}
```

**Parameters**:

- `dim_probe`: The key of the mapping to extract the dimensionality from.

### Data Source Mapping

Use an external data source to read the values to return.

```json
{
  "map_type": "DATA_SOURCE",
  "data_source": "source_name",
  "args": {
    "arg1": "value1",
    "arg2": true
  },
  "offset": 0,
  "scale": 1.0,
  "slice": "[0:10]",
  "comment": "Optional description"
}
```

**Parameters**:

- `data_source`: The name of the data source to use to read the data.
- `args`: A dictionary of arguments to pass to the data source reader.
- `offset` (optional): A postprocessing offset value to apply to the data.
- `scale` (optional): A postprocessing scale factior to apply to the data.
- `slice` (optional): Postprocessing slicing of the returned data.

### Expression Mapping
```json
{
  "map_type": "EXPR",
  "expr": "mathematical_expression",
  "parameters": {
    "param1": "value1"
  },
  "comment": "Optional description"
}
```

**Parameters**:

- `expr`: A mathematical expression to evaluate.
- `parameters`: A dictionary of parameters to pass into the expression.

### Uncertainty Mapping

Takes a reference signal and generates an uncertainty for each point and returns. The result has the same shape as the reference.

```json
{
  "map_type": "UNCERTAINTY",
  "model_type": "ABSOLUTE",
  "reference": "mapping_name",
  "absolute": 0.01,
  "comment": "Optional description"
}
{
  "map_type": "UNCERTAINTY",
  "model_type": "RELATIVE",
  "reference": "mapping_name",
  "relative": 0.01,
  "comment": "Optional description"
}
{
  "map_type": "UNCERTAINTY",
  "model_type": "MAX_ABS_REL",
  "reference": "mapping_name",
  "absolute": 0.01,
  "relative": 0.05,
  "comment": "Optional description"
}
```

**Parameters**:

- `model_type`: How the uncertainty is calculated, one of:
    - `ABSOLUTE`: `absolute` at every point (requires `absolute`).
    - `RELATIVE`: `relative * |x|` (requires `relative`).
    - `MAX_ABS_REL`: `max(absolute, relative * |x|)` (requires both `absolute` and `relative`).
- `reference`: The name of the mapping to calculate the uncertainty for.
- `absolute`: Absolute uncertainty, in the units of the reference (>= 0).
- `relative`: Relative uncertainty as a fraction, eg. `0.05` for 5% (>= 0).
# Note, may want to change this to percentage uncertainty at some point.

Parameters not used by the chosen `model_type` are not allowed, all checks are enforced by the schemas.

### Custom Function Mapping

If functionality is required that can be handled by any of the other mapping types then you can offload to a custom 
function that you expect to be available to run your mappings.

This should be used as a last resort and if you believe the functionality should be generally available please raise an
issue to request a new mapping type or extension to the current ones.

```json
{
  "map_type": "CUSTOM",
  "library": "library_name",
  "function": "function_name",
  "inputs": {
    "input1": "value1"
  },
  "comment": "Optional description"
}
```

**Parameters**:

- `library`: The name of the library of custom functionality.
- `function`: The function within the library to call.
- `inputs`: A dictionary of inputs to the function - either static values or other the results of other mappings.
