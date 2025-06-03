include(FetchContent)

set(SOCKPP_BUILD_SHARED OFF CACHE BOOL "Build shared sockpp library" FORCE)
set(SOCKPP_BUILD_STATIC ON CACHE BOOL "Build static sockpp library" FORCE)
FetchContent_Declare(
    sockpp
    GIT_REPOSITORY https://github.com/fpagliughi/sockpp
    GIT_TAG afdeacba9448c7a77194eed6ab8e1c0b1653c79a
)
FetchContent_MakeAvailable(sockpp)
add_library(irsol::sockpp ALIAS sockpp-static)