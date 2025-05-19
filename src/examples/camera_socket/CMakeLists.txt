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

if(IRSOL_USE_OPENCV)

add_executable(
    ${PROJECT_NAME}-viewer
    viewer_client.cpp
)

target_link_libraries(${PROJECT_NAME}-viewer
    irsol::cv
)

else()
message(STATUS, "OpenCV not found, viewer target skipped")
endif()