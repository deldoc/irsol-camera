cmake_minimum_required(VERSION 3.13.0)

# set project properties
project(simple)

add_executable(
    ${PROJECT_NAME}
    simple.cpp
)

target_link_libraries(${PROJECT_NAME}
    irsol::core
)