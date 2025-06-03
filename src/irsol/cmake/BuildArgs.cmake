include(FetchContent)

set(ARGS_BUILD_EXAMPLE OFF CACHE BOOL "Build args examples" FORCE)
set(ARGS_BUILD_UNITTESTS OFF CACHE BOOL "Build args unittes" FORCE)
FetchContent_Declare(
    args
    GIT_REPOSITORY https://github.com/Taywee/args.git
    GIT_TAG 6.4.7
)
FetchContent_MakeAvailable(args)
add_library(irsol::args ALIAS args)