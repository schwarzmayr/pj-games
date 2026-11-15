cmake_minimum_required(VERSION 3.16)
project(sdl3_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3
find_package(SDL3 REQUIRED)

add_executable(sdl3_demo src/main.cpp)
target_link_libraries(sdl3_demo PRIVATE SDL3::SDL3)
