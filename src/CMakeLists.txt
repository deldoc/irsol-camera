cmake_minimum_required(VERSION 3.13.0)

project("irsol-camera")

# Check the build type
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, defaulting to Release.")
    set(CMAKE_BUILD_TYPE Release)
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

# If building in Debug mode, add all the warning flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Adding debug flags.")
    string(APPEND CMAKE_CXX_FLAGS_DEBUG " -Wall -Wextra -pedantic")
endif()

# set up some cxx compiler flags
# enable C++14 support
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(external/spdlog)
add_subdirectory(external/neoapi)
add_subdirectory(irsol)
add_subdirectory(examples)
