message(STATUS "Also building irsol unit tests")

set(UNIT_TEST_DIR ${PROJECT_SOURCE_DIR})
include(${CMAKE_CURRENT_LIST_DIR}/cmake/FetchDependencies.cmake)

add_executable(unit_tests
  main.cpp
  protocol/message/test_assignment.cpp
  protocol/message/test_binary.cpp
  protocol/message/test_command.cpp
  protocol/message/test_error.cpp
  protocol/message/test_inquiry.cpp
  protocol/message/test_success.cpp
  protocol/message/test_variants.cpp
  protocol/parsing/test_parser.cpp
  protocol/parsing/test_parser_result.cpp
  protocol/serialization/test_serializer.cpp
  protocol/test_utils.cpp
  test_queue.cpp
  test_utils.cpp
)

target_link_libraries(unit_tests PRIVATE
  irsol::core
  Catch2::Catch2
)

include(Catch)
