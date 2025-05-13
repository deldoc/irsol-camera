cmake_minimum_required(VERSION 3.9.3)

# set project properties
project(edgedetect_cuda)

include_directories(
    SYSTEM
    ${REPO_ROOT_DIR}/external/include
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
    edgedetect_cuda.cpp)

target_link_libraries(${PROJECT_NAME}
    neoapi_cpp
    ${OpenCV_LIBS}
)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} pthread)
endif()
