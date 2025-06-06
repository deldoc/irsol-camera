cmake_minimum_required(VERSION 3.13.0)

# set project properties
project(06-client-server-interaction-image-commands)

add_executable(
    ${PROJECT_NAME}-server
    server.cpp
)

target_link_libraries(${PROJECT_NAME}-server
    irsol::core
)

if(IRSOL_USE_OPENCV)

add_executable(
    ${PROJECT_NAME}-viewer-gi
    viewer_client_gi.cpp
)

target_link_libraries(${PROJECT_NAME}-viewer-gi
    irsol::cv
)

add_executable(
    ${PROJECT_NAME}-viewer-gis
    viewer_client_gis.cpp
)

target_link_libraries(${PROJECT_NAME}-viewer-gis
    irsol::cv
)
else()
message(STATUS, "OpenCV not found, viewer target skipped")
endif()