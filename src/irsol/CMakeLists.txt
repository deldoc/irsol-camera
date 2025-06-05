# Set the project name
project (irsol)

# Find and link threading library for Linux
if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    message(STATUS "Threading support enabled for Linux.")
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmake/FetchDependencies.cmake)

# Core irsol library
add_library(${PROJECT_NAME}
    lib/irsol/assert.cpp
    lib/irsol/camera/interface.cpp
    lib/irsol/camera/discovery.cpp
    lib/irsol/camera/monitor.cpp
    lib/irsol/logging.cpp
    lib/irsol/utils.cpp
    lib/irsol/protocol/message/assignment.cpp
    lib/irsol/protocol/message/binary.cpp
    lib/irsol/protocol/message/command.cpp
    lib/irsol/protocol/message/error.cpp
    lib/irsol/protocol/message/inquiry.cpp
    lib/irsol/protocol/message/success.cpp
    lib/irsol/protocol/message/variants.cpp
    lib/irsol/protocol/parsing/parser.cpp
    lib/irsol/protocol/serialization/serializer.cpp
    lib/irsol/protocol/serialization/serialized_message.cpp
    lib/irsol/server/app.cpp
    lib/irsol/server/acceptor.cpp
    lib/irsol/server/image_collector/collector.cpp
    lib/irsol/server/client/session.cpp
    lib/irsol/server/client/state.cpp
    lib/irsol/server/message_handler.cpp
    lib/irsol/server/handlers/assignment_frame_rate.cpp
    lib/irsol/server/handlers/assignment_input_sequence_length.cpp
    lib/irsol/server/handlers/assignment_integration_time.cpp
    lib/irsol/server/handlers/base.cpp
    lib/irsol/server/handlers/command_abort.cpp
    lib/irsol/server/handlers/command_gi_base.cpp
    lib/irsol/server/handlers/command_gi.cpp
    lib/irsol/server/handlers/command_gis.cpp
    lib/irsol/server/handlers/inquiry_frame_rate.cpp
    lib/irsol/server/handlers/inquiry_integration_time.cpp
    lib/irsol/server/handlers/inquiry_input_sequence_length.cpp
)
add_library(irsol::core ALIAS ${PROJECT_NAME})

# Add debug flags and definitions if in Debug mode
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Adding debug flags and definitions.")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

# Compilation options, based on build type
set(IRSOL_DISABLE_ASSERT_OPTION false)
if(CMAKE_BUILD_TYPE  MATCHES Release)
    set(IRSOL_DISABLE_ASSERT_OPTION true)
endif()

if(IRSOL_DISABLE_ASSERT_OPTION)
    message(STATUS "Irsol assertions are disabled.")
    target_compile_definitions(${PROJECT_NAME} PRIVATE IRSOL_DISABLE_ASSERT)
else()
    message(STATUS "Irsol assertions are enabled.")
endif()

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    irsol::args
    irsol::neoapi
    irsol::ppk_assert
    irsol::sockpp
    irsol::spdlog
    irsol::tabulate
    Threads::Threads
)

# OpenCV irsol library
find_package(OpenCV)
if(NOT OpenCV_FOUND)
    message(STATUS "OpenCV not found. irsol-cv library will not be built.")
    return()
endif()

message(STATUS "Irsol built with OpenCV support, version: ${OpenCV_VERSION}")
set(IRSOL_USE_OPENCV ON PARENT_SCOPE)

add_library(${PROJECT_NAME}-cv
    lib/irsol/opencv.cpp
)
add_library(irsol::cv ALIAS ${PROJECT_NAME}-cv)

target_include_directories(${PROJECT_NAME}-cv
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}-cv
    ${PROJECT_NAME}
    ${OpenCV_LIBS}
)

target_compile_definitions(${PROJECT_NAME}-cv
    PUBLIC
        IRSOL_USE_OPENCV
)
