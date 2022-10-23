cmake_minimum_required(VERSION 3.12.0)

# Name of project
project(graphcalc VERSION 1.0)

# include_directories("${PROJECT_SOURCE_DIR}")

# Executable and dependencies
add_executable(graphcalc src/graphcalc.cpp src/graphcalc.hpp src/funcDeclarations.hpp src/graphs.hpp src/graphs.cpp)

