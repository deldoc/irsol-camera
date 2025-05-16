cmake_minimum_required(VERSION 3.13.0)

project(neoapi_examples)

add_subdirectory(simple)
add_subdirectory(user_buffer)
add_subdirectory(simple_socket)
add_subdirectory(camera_socket)

if(IRSOL_USE_OPENCV)
    add_subdirectory(simple_opencv)
endif()
