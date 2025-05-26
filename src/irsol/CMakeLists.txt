# Set the project name
project (irsol)

# Find and link threading library for Linux
if(UNIX AND NOT APPLE)
    find_package(Threads REQUIRED)
    message(STATUS "Threading support enabled for Linux.")
endif()

find_library(NEOAPI_LIBRARY neoapi_cpp HINTS ${neoapi_PRECOMPILED_LIBS_ROOT_DIR})

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
    lib/irsol/server/client_session.cpp
    lib/irsol/server/collector.cpp
    lib/irsol/server/message_handler.cpp
    lib/irsol/server/image_data.cpp
    lib/irsol/server/handlers/inquiry_fr.cpp
)
add_library(irsol::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    Sockpp::sockpp
    spdlog::spdlog
    neoapi::neoapi
    ppk_assert::ppk_assert
    tabulate::tabulate
    ${NEOAPI_LIBRARY}
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
