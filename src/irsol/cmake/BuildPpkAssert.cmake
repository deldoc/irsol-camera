get_filename_component(PPKASSERT_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../external/ppk_assert" ABSOLUTE)
get_filename_component(PPKASSERT_INCLUDE_DIR "${PPKASSERT_ROOT_DIR}/include" ABSOLUTE)
get_filename_component(PPKASSERT_SOURCE_DIR "${PPKASSERT_ROOT_DIR}/lib/ppk_assert" ABSOLUTE)

if (NOT EXISTS ${PPKASSERT_ROOT_DIR})
message(FATAL_ERROR, "PPKASSERT_ROOT_DIR '${PPKASSERT_ROOT_DIR}' does not exist")
endif()
if(NOT EXISTS "${PPKASSERT_INCLUDE_DIR}")
message(FATAL_ERROR "PPKASSERT_INCLUDE_DIR '${PPKASSERT_INCLUDE_DIR}' does not exist")
endif()


add_library(
    ppkassert
    ${PPKASSERT_SOURCE_DIR}/ppk_assert.cpp
    )
    
target_include_directories(ppkassert
    PUBLIC ${PPKASSERT_INCLUDE_DIR}
)
add_library(irsol::ppk_assert ALIAS ppkassert)
