cmake_minimum_required(VERSION 3.13.0)

# set project properties
project(simple-socket)

add_executable(
    ${PROJECT_NAME}-server
    socket_server.cpp
)

target_link_libraries(${PROJECT_NAME}-server
    irsol::core
)

add_executable(
    ${PROJECT_NAME}-client
    socket_client.cpp
)

target_link_libraries(${PROJECT_NAME}-client
    irsol::core
)