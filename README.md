# exactly-once

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
$ bin/main
```

#### Optional arguments

- `-i ID`, node identifier
- `-p PORT`, port to listen on
- `-c ID:IP:PORT`, connect to node with id `ID`, with ip `IP`, listening on port `PORT`
- `-f FANOUT`, fanout (if `-f 0` it behaves as a flood algorithm)
- `-t INTERVAL`, synchronization interval (milliseconds)

The defaults are:
- `-i 0`
- `-p 3000`
- `-f 1`
- `-t 1000`

#### Example

Three nodes with identifiers `0`, `1` and `2`, all connected:

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
