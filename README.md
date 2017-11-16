[![Build Status](https://travis-ci.org/vitorenesduarte/exactly-once.svg?branch=master)](https://travis-ci.org/vitorenesduarte/exactly-once)

# exactly-once

### Assumptions
- ids are integers
- max UDP datagram size is `2^16 = 65536`

### Build
```bash
$ make
```

### Docker build
```bash
$ make docker
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

#### Example using Docker Compose
```
$ docker-compose up
```

#### Chaos testing with [pumba](https://github.com/gaia-adm/pumba)

After running `docker-compose up`,

- packet loss
```bash
$ pumba netem --tc-image gaiadocker/iproute2 --duration 5m --interface eth0 loss --percent 50
```
- packet delay
```bash
$ pumba netem --tc-image gaiadocker/iproute2 --duration 5m --interface eth0 delay --time 1000  --jitter 1000
```

([tutorial on pumba](https://hackernoon.com/network-emulation-for-docker-containers-f4d36b656cc3))
