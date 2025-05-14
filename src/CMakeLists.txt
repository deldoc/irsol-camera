cmake_minimum_required(VERSION 3.13)

project("irsol-camera" VERSION 1.0 LANGUAGES CXX)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, defaulting to Release.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable position-independent code globally
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Output directories for build artifacts
set(TARGET_DIR_NAME $<LOWER_CASE:${CMAKE_BUILD_TYPE}>)
set(DIST_TARGET_DIRECTORY ${CMAKE_SOURCE_DIR}/dist/${TARGET_DIR_NAME})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${DIST_TARGET_DIRECTORY}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${DIST_TARGET_DIRECTORY}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${DIST_TARGET_DIRECTORY}/bin)

# Add debug flags and definitions if in Debug mode
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Adding debug flags and definitions.")
    add_compile_options(-Wall -Wextra -pedantic)
    add_compile_definitions(DEBUG)
endif()

# Compilation options, based on build type
set(IRSOL_DISABLE_ASSERT_OPTION false)
if(CMAKE_BUILD_TYPE  MATCHES Release)
    set(IRSOL_DISABLE_ASSERT_OPTION true)
endif()

if(IRSOL_DISABLE_ASSERT_OPTION)
    message(STATUS "Irsol assertions are disabled.")
    add_compile_definitions(IRSOL_DISABLE_ASSERT)
else()
    message(STATUS "Irsol assertions are enabled.")
endif()


# Log compiler information
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

# Add subdirectories for external dependencies
add_subdirectory(external/spdlog)
add_subdirectory(external/neoapi)
add_subdirectory(external/ppk_assert)

# Add subdirectories for the main project
add_subdirectory(irsol)

# Add subdirectories for examples
add_subdirectory(examples)
