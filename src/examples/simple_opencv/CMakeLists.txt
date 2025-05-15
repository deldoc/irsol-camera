cmake_minimum_required(VERSION 3.13.0)

# set project properties
project(simple_opencv)

add_executable(
    ${PROJECT_NAME}
    simple_opencv.cpp
)

target_link_libraries(${PROJECT_NAME}
    irsol::cv
)