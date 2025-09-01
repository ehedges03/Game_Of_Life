# Explicitly list source files
set(LIBFUNNI_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libFunni.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.cpp
)

# Define library
add_library(LibFunni ${LIBFUNNI_SRCS})

# Include headers
target_include_directories(LibFunni 
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/private
)

# TODO: determine how cmake install interfaces might be used to require (or allow) the main
#       project and test to include library headers with a prefix like: #include <libproj/file.h>
#       without requiring that the build directory reflect the same (e.g. LibProj/public/libproj/file.h)
#       Right now there is no ability or requirement for dependencies to do anything more specific than: 
#       #include <file.h>
