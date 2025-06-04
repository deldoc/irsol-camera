cmake_minimum_required(VERSION 3.13)

project("irsol-camera" VERSION 1.0 LANGUAGES CXX)

add_subdirectory("src")

# Check if  IRSOL_BUILD_DOCS is defined and set to ON
option(IRSOL_BUILD_DOCS "Build irsol documentation" ON)
if(IRSOL_BUILD_DOCS)
    add_subdirectory(docs)
else()
    message(STATUS "IRSOL_BUILD_DOCS is OFF. Skipping building irsol documentation.")
endif()
