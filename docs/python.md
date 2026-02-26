## Installation

### From PyPI

```bash
pip install tokamap
```

### From Source

```bash
git clone https://github.com/ukaea/tokamap.git
pip install .
```

## TokaMap CLI

Finding the schemas directory:

```bash
tokamap --schemas-dir
```

Checking the installed version:

```bash
tokamap --version
```

## TokaMap library

Finding the schemas directory:

```python
from tokamap import schemas_dir

print(schemas_dir)
```

Checking the installed version:

```python
from tokamap import __version__

print(__version__)
```
