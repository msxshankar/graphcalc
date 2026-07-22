cmake_minimum_required(VERSION 3.14.0)

project(graphcalc CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2")

# Source files
file(GLOB SOURCES "src/*.cpp")

# Executable and dependencies
add_executable(graphcalc ${SOURCES})
target_link_libraries(graphcalc PRIVATE X11 GL)

install(TARGETS graphcalc DESTINATION bin)

