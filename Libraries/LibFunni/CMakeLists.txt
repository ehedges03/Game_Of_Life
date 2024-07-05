# Explicitly list source files
set(libfunni_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LibFunni/libFunni.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/LibFunni/pch.cpp
)

# Define library
add_library(LibFunni ${libfunni_sources})

# Include headers
target_include_directories(LibFunni PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

