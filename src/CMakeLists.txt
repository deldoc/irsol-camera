cmake_minimum_required(VERSION 3.13)

project("irsol-camera" VERSION 1.0 LANGUAGES CXX)

# Set default build type to Release if not specified
if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, defaulting to Release.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable position-independent code globally
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Output directories for build artifacts
foreach(OUTPUTCONFIG DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    string(TOLOWER "${OUTPUTCONFIG}" OUTPUTCONFIG_LOWER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/dist/${OUTPUTCONFIG_LOWER}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/dist/${OUTPUTCONFIG_LOWER}/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${CMAKE_SOURCE_DIR}/dist/${OUTPUTCONFIG_LOWER}/lib")
endforeach()

# Log compiler information
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

# Add the main project sub-directory
add_subdirectory(irsol)


# Check if IRSOL_BUILD_EXAMPLES is defined and set to ON
option(IRSOL_BUILD_EXAMPLES "Build irsol examples" ON)
if(IRSOL_BUILD_TESTS)
    add_subdirectory(examples)
else()
    message(STATUS "IRSOL_BUILD_EXAMPLES is OFF. Skipping building irsol examples.")
endif()

# Check if IRSOL_BUILD_TESTS is defined and set to ON
option(IRSOL_BUILD_TESTS "Build irsol tests" OFF)
if(IRSOL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
else()
    message(STATUS "IRSOL_BUILD_TEST is OFF. Skipping building irsol tests.")
endif()

