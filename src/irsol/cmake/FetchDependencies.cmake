# args library for command-line parsing
include("${CMAKE_CURRENT_LIST_DIR}/BuildArgs.cmake")

# neoapi library for camera-interaction
include("${CMAKE_CURRENT_LIST_DIR}/BuildNeoAPI.cmake")

# ppkassert library for assertions
include("${CMAKE_CURRENT_LIST_DIR}/BuildPpkAssert.cmake")

# sockpp socket library
include("${CMAKE_CURRENT_LIST_DIR}/BuildSockpp.cmake")

# spdlog library for logging
include("${CMAKE_CURRENT_LIST_DIR}/BuildSpdlog.cmake")

# tabulate library for tabular printing
include("${CMAKE_CURRENT_LIST_DIR}/BuildTabulate.cmake")
