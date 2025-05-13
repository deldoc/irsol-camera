cmake_minimum_required(VERSION 3.9.3)

# set project properties
project(getting_started)

include_directories(
    SYSTEM
    ${REPO_ROOT_DIR}/external/include
)

add_executable(${PROJECT_NAME}
    getting_started.cpp)

target_link_libraries(${PROJECT_NAME}
    neoapi_cpp
)
