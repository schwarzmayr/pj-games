cmake_minimum_required(VERSION 3.5)

project(hello)

# Allow for using folders in Visual Studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set C++20 as the standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Make Windows headers use wide character set
add_definitions(/D UNICODE /D _UNICODE)


# download CPM.cmake, which makes version control easier to handle for CMake Fetch
file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.40.8/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=78ba32abdf798bc616bab7c73aac32a17bbd7b06ad9e26a6add69de8f3ae4791
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# set the output directory for built objects.
# This makes sure that the dynamic library goes into the build directory automatically.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendored/SDL")
	# This assumes you have added SDL as a submodule in vendored/SDL
	add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
else()
	# Download package
	CPMAddPackage("gh:libsdl-org/SDL#f00dc91")
endif()

# Create your game executable target as usual
add_executable(hello WIN32 hello.cpp)

# Link to the actual SDL3 library.
target_link_libraries(hello PRIVATE SDL3::SDL3)