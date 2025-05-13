cmake_minimum_required(VERSION 3.9.3)

if(OPENCV_HIGHGUI_FOUND)
    # set project properties
    project(opencv_cap)

    include_directories(
        SYSTEM
        ${REPO_ROOT_DIR}/external/include
        ${OpenCV_INCLUDE_DIRS}
    )

    add_executable(${PROJECT_NAME}
        opencv_cap.cpp)

    target_link_libraries(${PROJECT_NAME}
        neoapi_cpp
        ${OpenCV_LIBS}
    )
endif()
