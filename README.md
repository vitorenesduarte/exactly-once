# exactly-once

### Requirements

- [CMake](https://cmake.org/)

```bash
$ export HUNTER_ROOT=~/.hunter
```

### Build
```bash
$ make
```

### Run tests
```bash
$ make test
```

### Run echo server
```bash
$ bin/main
```

```bash
$ nc -4u localhost 3000
```

### Assumptions
- ids are integers
- max UDP datagram size is `2^16 = 65536`
