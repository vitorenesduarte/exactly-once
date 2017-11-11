cmake_minimum_required(VERSION 3.0)

# use C++ 11
set (CMAKE_CXX_STANDARD 11)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.145.tar.gz"
    SHA1 "8ff59de9118c340fa3147ec9ff951247503e45b8"
)

project(exactly-once)

# binaries in bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin/)

# boost
hunter_add_package(Boost COMPONENTS system thread)
find_package(Boost CONFIG REQUIRED system thread)

# msgpack
hunter_add_package(msgpack)
find_package(msgpack CONFIG REQUIRED)

# build test
add_executable(simple_test test/simple_test.cc)
target_link_libraries(simple_test msgpack::msgpack)
# build main
add_executable(main src/peer_service.cc src/main.cc)
target_link_libraries(main Boost::boost
                           Boost::system
                           Boost::thread
                           msgpack::msgpack)
