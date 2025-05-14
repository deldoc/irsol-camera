cmake_minimum_required(VERSION 3.13.0)

# set project properties
project(user_buffer)

find_library(NEOAPI_LIBRARY neoapi_cpp HINTS ${neoapi_PRECOMPILED_LIBS_ROOT_DIR})

add_executable(${PROJECT_NAME}
    user_buffer.cpp)

target_link_libraries(${PROJECT_NAME}
    spdlog::spdlog
    neoapi::neoapi
    ${NEOAPI_LIBRARY}
)
