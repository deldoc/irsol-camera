message(STATUS "Building irsol unit tests")

# --- Get Catch2 ---
set(UNIT_TEST_DIR ${PROJECT_SOURCE_DIR})

include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.8.1
)
FetchContent_MakeAvailable(Catch2)

add_executable(unit_tests
  main.cpp
  test_protocol_message.cpp
  test_protocol_parser.cpp
  test_protocol_serializer.cpp
  test_protocol_utils.cpp
)

target_link_libraries(unit_tests PRIVATE
  irsol::core
  args::args
  Catch2::Catch2
)

include(Catch)
