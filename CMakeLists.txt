cmake_minimum_required(VERSION 3.14.0)

# Name of project
project(graphcalc)

# Add files from src
file(GLOB SOURCES "src/*.cpp")

# Add matplotplusplus directory
add_subdirectory(matplotplusplus)

# Executable and dependencies
add_executable(graphcalc ${SOURCES})

# Link matplotplusplus library
target_link_libraries(graphcalc PUBLIC matplot)
