cmake_minimum_required(VERSION 3.9.3)

if(OPENCV_HIGHGUI_FOUND)
    # set project properties
    project(opencv)

    include_directories(
        SYSTEM
        ${NEOAPI_ROOT_DIR}/include
        ${OpenCV_INCLUDE_DIRS}
    )

    add_executable(${PROJECT_NAME}
        opencv.cpp)

    target_link_libraries(${PROJECT_NAME}
        neoapi_cpp
        ${OpenCV_LIBS}
    )
endif()
