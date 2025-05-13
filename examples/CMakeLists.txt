cmake_minimum_required(VERSION 3.9.3)

project(neoapi_examples)

# Cmake is searching for OpenCV which is required for some examples. See README for further information.
option(OpenCV_STATIC "Link with static version of OpenCV without Visual Studio runtime libraries" ON)
find_package(OpenCV QUIET COMPONENTS core highgui imgproc imgcodecs videoio
                          OPTIONAL_COMPONENTS cudaimgproc)


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
# enable C++11 support
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "CMAKE_CXX_FLAGS_RELEASE: ${CMAKE_CXX_FLAGS_RELEASE}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Determine the build output prefix based on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_OUTPUT_PREFIX debug)
else()
    set(BUILD_OUTPUT_PREFIX release)
endif()

# Configure output directory for created binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${BUILD_OUTPUT_PREFIX}/bin)

# add the lib folder to the link path
get_filename_component(NEOAPI_ROOT_DIR ${PROJECT_SOURCE_DIR}/.. ABSOLUTE)
if(NOT EXISTS "${NEOAPI_ROOT_DIR}/lib/neoapi")
    message(FATAL_ERROR "NEOAPI_ROOT_DIR does not contain the required lib/neoapi folder. Please check your installation.")
endif()
link_directories(${NEOAPI_ROOT_DIR}/lib/neoapi)

# Add each example to the project with the appropriate build directory
add_subdirectory(getting_started ${BUILD_OUTPUT_PREFIX}/build/getting_started)
add_subdirectory(user_buffer ${BUILD_OUTPUT_PREFIX}/build/user_buffer)
if(OpenCV_FOUND AND "${OpenCV_VERSION_MAJOR}" GREATER "2")
    add_subdirectory(opencv ${BUILD_OUTPUT_PREFIX}/build/opencv)
    add_subdirectory(opencv_cap ${BUILD_OUTPUT_PREFIX}/build/opencv_cap)
    add_subdirectory(edgedetect_opencl ${BUILD_OUTPUT_PREFIX}/build/edgedetect_opencl)
    if (${OPENCV_CUDAIMGPROC_FOUND})
        add_subdirectory(edgedetect_cuda ${BUILD_OUTPUT_PREFIX}/build/edgedetect_cuda)
    endif()
else()
    message(STATUS "OpenCV was not found. Some Examples are disabled. Please provide OpenCV_DIR during cmake call to solve this.")
    set(OpenCV_FOUND 0)
endif()
