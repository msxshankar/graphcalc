cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)

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
#file(GLOB TEST "test/*.cpp")
#add_executable(graphcalc_test ${TEST})

#add_test(
#       NAME graphcalc_test
# COMMAND $<TARGET_FILE:graphcalc_test>
#)

# Google Test
include(FetchContent)
FetchContent_Declare(
        googletest
        # Specify the commit you depend on and update it regularly.
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add graphcalc test
file(GLOB TEST "test/*.cpp")
add_executable(graphcalc_test ${TEST} ${SOURCES})
target_link_libraries(graphcalc_test GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(graphcalc_test)
add_test(
        NAME graphcalc_test
        COMMAND $<TARGET_FILE:graphcalc_test>
)