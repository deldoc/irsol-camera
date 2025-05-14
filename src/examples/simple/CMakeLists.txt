cmake_minimum_required(VERSION 3.13.0)

# set project properties
project(simple)


find_library(NEOAPI_LIBRARY neoapi_cpp HINTS ${neoapi_PRECOMPILED_LIBS_ROOT_DIR})

add_executable(
    ${PROJECT_NAME}
    simple.cpp
)

target_link_libraries(${PROJECT_NAME}
    neoapi::neoapi
    irsol::irsol
    ${NEOAPI_LIBRARY}
)