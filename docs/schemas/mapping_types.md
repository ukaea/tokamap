#### Dimension Mapping
```json
{
  "MAP_TYPE": "DIMENSION",
  "DIM_PROBE": "probe_identifier",
  "COMMENT": "Optional description"
}
```

#### Value Mapping
```json
{
  "MAP_TYPE": "VALUE",
  "VALUE": "any_value_type",
  "COMMENT": "Optional description"
}
```

#### Data Source Mapping
```json
{
  "MAP_TYPE": "DATA_SOURCE",
  "DATA_SOURCE": "source_name",
  "ARGS": {
    "arg1": "value1",
    "arg2": true
  },
  "OFFSET": 0,
  "SCALE": 1.0,
  "SLICE": "0:10",
  "COMMENT": "Optional description"
}
```

#### Expression Mapping
```json
{
  "MAP_TYPE": "EXPR",
  "EXPR": "mathematical_expression",
  "PARAMETERS": {
    "param1": "value1"
  },
  "COMMENT": "Optional description"
}
```

#### Custom Function Mapping
```json
{
  "MAP_TYPE": "CUSTOM",
  "LIBRARY": "library_name",
  "FUNCTION": "function_name",
  "INPUTS": {
    "input1": "value1"
  },
  "PARAMETERS": {},
  "COMMENT": "Optional description"
}
```
