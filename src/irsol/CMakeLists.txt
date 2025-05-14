# Set the project name
project (irsol)

find_library(NEOAPI_LIBRARY neoapi_cpp HINTS ${neoapi_PRECOMPILED_LIBS_ROOT_DIR})

add_library(${PROJECT_NAME}
    lib/irsol/assert.cpp
    lib/irsol/logging.cpp
    lib/irsol/utils.cpp
)
add_library(irsol::irsol ALIAS ${PROJECT_NAME})

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
