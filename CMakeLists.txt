cmake_minimum_required(VERSION 3.10)

# set the project name
project(Chip8Emulator VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

configure_file(Chip8Config.h.in Chip8Config.h)

# add the executable
add_executable(chip8 src/chip8.cpp)
target_include_directories(chip8 PUBLIC build)
