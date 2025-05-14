# Set the project name
project (neoapi)

add_library(
    ${PROJECT_NAME}
    INTERFACE
    )
add_library(neoapi::neoapi ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE ${PROJECT_SOURCE_DIR}/include
)

# Export the path to the `lib` folder, so that this can be used in other CMakeLists.txt files
get_filename_component(neoapi_PRECOMPILED_LIBS_ROOT_DIR ${PROJECT_SOURCE_DIR}/lib/neoapi ABSOLUTE)
if(NOT EXISTS "${neoapi_PRECOMPILED_LIBS_ROOT_DIR}")
    message(FATAL_ERROR "neoapi_PRECOMPILED_LIBS_ROOT_DIR does not contain the required lib/neoapi folder. Please check your installation.")
endif()
set(neoapi_PRECOMPILED_LIBS_ROOT_DIR ${neoapi_PRECOMPILED_LIBS_ROOT_DIR} PARENT_SCOPE)