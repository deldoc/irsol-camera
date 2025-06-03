
get_filename_component(NEOAPI_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/neoapi" ABSOLUTE)
get_filename_component(NEOAPI_INCLUDE_DIR "${NEOAPI_ROOT_DIR}/include" ABSOLUTE)
get_filename_component(NEOAPI_PRECOMPILED_LIB_PATH "${NEOAPI_ROOT_DIR}/lib/neoapi/libneoapi_cpp.so" ABSOLUTE)
if (NOT EXISTS ${NEOAPI_ROOT_DIR})
message(FATAL_ERROR, "NEO_API_ROOT_DIR '${NEOAPI_ROOT_DIR}' does not exist")
endif()
if(NOT EXISTS "${NEOAPI_INCLUDE_DIR}")
message(FATAL_ERROR "NEOAPI_INCLUDE_DIR '${NEOAPI_INCLUDE_DIR}' does not exist")
endif()
if(NOT EXISTS "${NEOAPI_PRECOMPILED_LIB_PATH}")
message(FATAL_ERROR "NEOAPI_PRECOMPILED_LIB_PATH '${NEOAPI_PRECOMPILED_LIB_PATH}' does not point to required precompiled shared lib")
endif()

add_library(neoapi INTERFACE)
target_include_directories(neoapi
    INTERFACE
        ${NEOAPI_INCLUDE_DIR}
)

add_library(neoapi::precompiled_libs SHARED IMPORTED)
set_target_properties(neoapi::precompiled_libs PROPERTIES
    IMPORTED_LOCATION "${NEOAPI_PRECOMPILED_LIB_PATH}"
    INTERFACE_INCLUDE_DIRECTORIES "${NEOAPI_INCLUDE_DIR}"
)

# Link precompiled libs to the main neoapi interface target
target_link_libraries(neoapi INTERFACE neoapi::precompiled_libs)
add_library(irsol::neoapi ALIAS neoapi)