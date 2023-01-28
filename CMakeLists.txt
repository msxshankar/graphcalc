cmake_minimum_required(VERSION 3.14.0)

# Name of project
project(graphcalc)

# Testing
enable_testing()

# Add files from src
file(GLOB SOURCES "src/*.cpp")

# Add matplotplusplus directory
add_subdirectory(matplotplusplus)

# Executable and dependencies
add_executable(graphcalc ${SOURCES})

# Link matplotplusplus library
target_link_libraries(graphcalc PUBLIC matplot)

# Add files from test
file(GLOB TEST "test/*.cpp")
add_executable(graphcalc_test ${TEST})

add_test(
        NAME graphcalc_test
        COMMAND $<TARGET_FILE:graphcalc_test>
)
