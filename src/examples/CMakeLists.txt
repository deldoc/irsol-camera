cmake_minimum_required(VERSION 3.13.0)

# Add debug flags and definitions if in Debug mode
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Adding debug flags and definitions.")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
    )
    add_compile_definitions(DEBUG)
endif()


add_subdirectory(00-logging-and-asserting)
add_subdirectory(01-loading-images)
add_subdirectory(02-interacting-with-camera-features)
add_subdirectory(03-message-protocols)
add_subdirectory(04-message-handlers)
add_subdirectory(05-client-server-interaction)
add_subdirectory(06-client-server-interaction-image-commands)