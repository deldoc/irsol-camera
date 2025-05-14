project (ppk_assert)

add_library(
    ${PROJECT_NAME}
    lib/ppk_assert/ppk_assert.cpp
    )
add_library(ppk_assert::ppk_assert ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)
