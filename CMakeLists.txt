cmake_minimum_required(VERSION 3.0)

include("cmake/HunterGate.cmake")
HunterGate(
    URL "https://github.com/ruslo/hunter/archive/v0.19.145.tar.gz"
    SHA1 "8ff59de9118c340fa3147ec9ff951247503e45b8"
)

project(exactly-once)

# libevent
hunter_add_package(Libevent)
find_package(Libevent CONFIG REQUIRED)
add_executable(main handoff.cc)
target_link_libraries(main Libevent::event_core
                           Libevent::event_extra)
