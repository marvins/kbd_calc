# Version extraction and configuration
# Reads VERSION file, extracts git info, and configures version.hpp

# Read version from VERSION file
file(READ ${CMAKE_SOURCE_DIR}/VERSION VERSION_FILE)
string(STRIP ${VERSION_FILE} PROJECT_VERSION)

# Display version (may include -dirty suffix)
set(PROJECT_VERSION_DISPLAY ${PROJECT_VERSION})

# Parse version components
string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
list(GET VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET VERSION_LIST 2 PROJECT_VERSION_PATCH)

# Get git hash and dirty status
find_package(Git)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} diff-index --quiet HEAD --
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE GIT_DIRTY_RESULT
        ERROR_QUIET
    )
    if(GIT_DIRTY_RESULT EQUAL 0)
        set(GIT_DIRTY 0)
    else()
        set(GIT_DIRTY 1)
        set(PROJECT_VERSION_DISPLAY "${PROJECT_VERSION_DISPLAY}-dirty")
    endif()
else()
    set(GIT_HASH "unknown")
    set(GIT_DIRTY 0)
endif()

# Build date
string(TIMESTAMP BUILD_DATE "%Y-%m-%d %H:%M:%S UTC" UTC)

# Configure version.hpp
configure_file(
    ${CMAKE_SOURCE_DIR}/templates/version.hpp.in
    ${CMAKE_BINARY_DIR}/src/overboard/version.hpp
    @ONLY
)

