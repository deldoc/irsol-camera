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
  protocol/message/test_assignment.cpp
  protocol/message/test_binary.cpp
  protocol/message/test_command.cpp
  protocol/message/test_error.cpp
  protocol/message/test_inquiry.cpp
  protocol/message/test_success.cpp
  protocol/message/test_variants.cpp
  protocol/serialization/test_serializer.cpp
  protocol/test_parser.cpp
  protocol/test_utils.cpp
)

target_link_libraries(unit_tests PRIVATE
  irsol::core
  args::args
  Catch2::Catch2
)

include(Catch)
