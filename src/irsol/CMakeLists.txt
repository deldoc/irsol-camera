# Set the project name
project (irsol)

find_library(NEOAPI_LIBRARY neoapi_cpp HINTS ${neoapi_PRECOMPILED_LIBS_ROOT_DIR})

# Core irsol library
add_library(${PROJECT_NAME}
    lib/irsol/assert.cpp
    lib/irsol/logging.cpp
    lib/irsol/utils.cpp
)
add_library(irsol::core ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    spdlog::spdlog
    neoapi::neoapi
    ppk_assert::ppk_assert
    ${NEOAPI_LIBRARY}
)

# Opencv irsol library
find_package(OpenCV)
if(NOT OpenCV_FOUND)
    message(STATUS  
    "OpenCV not found. irsol-cv library will not be built.")
    return()
endif()

message(STATUS "Irsol built with opencv support, with version: ${OpenCV_VERSION}")
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
