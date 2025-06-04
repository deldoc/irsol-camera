include(FetchContent)
FetchContent_Declare(
    doxygen-awesome-css
    GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
    GIT_TAG v2.3.4
)
FetchContent_MakeAvailable(doxygen-awesome-css)

# Save the location the files were cloned into
# This allows us to get the path to doxygen-awesome.css
FetchContent_GetProperties(doxygen-awesome-css SOURCE_DIR AWESOME_CSS_DIR)

# Generate the Doxyfile
set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
set(DOXYFILE_OUT ${CMAKE_CURRENT_SOURCE_DIR}/generated/Doxyfile)
configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)