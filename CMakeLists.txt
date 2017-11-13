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

# msgpack
hunter_add_package(msgpack)
find_package(msgpack CONFIG REQUIRED)

# build main
add_executable(main src/main.cc)
target_link_libraries(main msgpack::msgpack)

# build tests
# - handoff
add_executable(handoff_test test/handoff_test.cc)
target_link_libraries(handoff_test msgpack::msgpack)
# - peer_service
add_executable(peer_service_test test/peer_service_test.cc)
target_link_libraries(peer_service_test msgpack::msgpack)
