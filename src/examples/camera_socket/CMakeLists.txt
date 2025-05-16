cmake_minimum_required(VERSION 3.13.0)

# set project properties
project(camera-socket)

add_executable(
    ${PROJECT_NAME}-server
    camera_server.cpp
)

target_link_libraries(${PROJECT_NAME}-server
    irsol::core
)
