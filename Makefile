ifndef HUNTER_ROOT
  $(error HUNTER_ROOT is not set)
endif

all: build

deps:
	cmake -H. -B_builds
	#cmake -H. -B_builds -DHUNTER_STATUS_DEBUG=ON -DCMAKE_BUILD_TYPE=Debug

build: deps
	cmake --build _builds
	#cmake --build _builds --config Debug
