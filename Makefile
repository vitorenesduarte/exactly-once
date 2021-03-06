all: build

deps:
	if [ ! -d msgpack-c ]; then git clone https://github.com/msgpack/msgpack-c; else cd msgpack-c && git pull; fi

build: deps
	g++ --std=c++11 -pthread -I msgpack-c/include test/handoff_test.cc -o bin/handoff_test
	g++ --std=c++11 -pthread -I msgpack-c/include test/peer_service_test.cc -o bin/peer_service_test
	g++ --std=c++11 -pthread -I msgpack-c/include test/pumba_test.cc -o bin/pumba_test
	g++ --std=c++11 -pthread -I msgpack-c/include src/main.cc -o bin/main

test: build
	bin/handoff_test
	bin/peer_service_test

docker:
	docker build --no-cache -t vitorenesduarte/exactly-once -f Dockerfiles/exactly-once .
	docker push vitorenesduarte/exactly-once
