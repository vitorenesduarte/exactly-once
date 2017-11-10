# exactly-once

### Requirements
```bash
$ export HUNTER_ROOT=~/.hunter
```

### Build
```bash
$ make
```

### Run echo server
```bash
$ _builds/main
```

```bash
$ nc -4u localhost 3000
```

### Run tests
```bash
$ _builds/simple_test
```

### Assumptions
- max UDP datagram size is 2^16 = 65536
