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

### Run server
```bash
$ bin/main -i 0 -p 3000 -c 1:127.0.0.1:3001 -c 2:127.0.0.1:3002
```
```bash
$ bin/main -i 1 -p 3001 -c 0:127.0.0.1:3000 -c 2:127.0.0.1:3002
```
```bash
$ bin/main -i 2 -p 3002 -c 0:127.0.0.1:3000 -c 1:127.0.0.1:3001
```

### Assumptions
- ids are integers
- max UDP datagram size is `2^16 = 65536`
