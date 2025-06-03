cmake_minimum_required(VERSION 3.13.0)

project(02-interacting-with-camera-features)

add_executable(
    ${PROJECT_NAME}
    main.cpp
)
target_compile_definitions(${PROJECT_NAME} PRIVATE PROGRAM_NAME="${PROJECT_NAME}")

target_link_libraries(${PROJECT_NAME}
    irsol::core
)