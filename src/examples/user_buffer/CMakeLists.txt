cmake_minimum_required(VERSION 3.13.0)

# set project properties
project(user_buffer)


add_executable(${PROJECT_NAME}
    user_buffer.cpp)

target_link_libraries(${PROJECT_NAME}
    irsol::core
)
