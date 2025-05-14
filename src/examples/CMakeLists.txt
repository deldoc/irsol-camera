cmake_minimum_required(VERSION 3.13.0)

project(neoapi_examples)

# Find OpenCV via the FindOpenCV module in the 'cmake' directory
find_package(OpenCV)
if(NOT OpenCV_FOUND)
    message(STATUS  
    "OpenCV not found. Some examples will not be available. "
    "If you want to use OpenCV, please install it following the instructions provided in the `docs/` folder. ")
endif()
message(STATUS "Found OpenCV version: ${OpenCV_VERSION}")

add_subdirectory(getting_started)
add_subdirectory(user_buffer)
add_subdirectory(simple)

if (OpenCV_FOUND)
    add_subdirectory(simple_opencv)
endif()