cmake_minimum_required(VERSION 3.13.0)

project(01-loading-images)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROGRAM_NAME="${PROJECT_NAME}")

target_link_libraries(${PROJECT_NAME}
    irsol::core
)

if(IRSOL_USE_OPENCV)
    add_executable(
    ${PROJECT_NAME}-opencv
    main-opencv.cpp
    )
    target_compile_definitions(${PROJECT_NAME}-opencv PRIVATE PROGRAM_NAME="${PROJECT_NAME}-opencv")

    target_link_libraries(${PROJECT_NAME}-opencv
        irsol::cv
    )
else()
    message(STATUS "Skipping building of ${PROJECT_NAME}-opencv, as opencv is not available")
endif()