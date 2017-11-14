all: build

deps:
	if [ ! -d msgpack-c ]; then git clone https://github.com/msgpack/msgpack-c; fi
	cd msgpack-c && git pull

build: deps
	g++ --std=c++11 -pthread -I msgpack-c/include test/handoff_test.cc -o bin/handoff_test
	g++ --std=c++11 -pthread -I msgpack-c/include test/peer_service_test.cc -o bin/peer_service_test
	g++ --std=c++11 -pthread -I msgpack-c/include src/main.cc -o bin/main

test: build
	bin/handoff_test
	bin/peer_service_test
