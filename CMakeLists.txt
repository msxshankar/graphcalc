cmake_minimum_required(VERSION 3.12.0)

# Name of project
project(graphcalc)

# Add files from src
file(GLOB SOURCES "src/*.cpp")

# Executable and dependencies
add_executable(graphcalc ${SOURCES})

