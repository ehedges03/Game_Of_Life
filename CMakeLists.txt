cmake_minimum_required(VERSION 3.10)

# Set project name
project(GameOfLife LANGUAGES C CXX)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Export compile commands for nvim lsp
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE src_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch]pp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch])

add_subdirectory(libraries)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

add_executable(${PROJECT_NAME} ${src_files})

# Link with FunniLib
target_link_libraries(${PROJECT_NAME} PRIVATE LibFunni PRIVATE glfw PRIVATE glad::glad)














# # List the files of the current local project 
# #    Default behavior: Automatically add all hpp and cpp files from src/ directory
# #    You may want to change this definition in case of specific file structure
# file(GLOB_RECURSE src_files ${CMAKE_CURRENT_LIST_DIR}/src/*.[ch]pp)
# 
# # Generate the executable_name from the current directory name
# get_filename_component(executable_name ${CMAKE_CURRENT_LIST_DIR} NAME)
# # Another possibility is to set your own name: set(executable_name your_own_name) 
# message(STATUS "Configure steps to build executable file [${executable_name}]")
# project(${executable_name})
# 
# # Add current src/ directory
# include_directories("src")
# 
# # Include files from the library (vcl as well as external dependencies)
# #  > The relative path to the VCL library may need to be adapted
# include("library/CMakeLists.txt")
# 
# 
# # Add all files to create executable
# #  @src_files: the local file for this project
# #  @src_files_vcl: all files of the VCL library
# #  @src_files_third_party: all third party libraries compiled with the project
# add_executable(${executable_name} ${src_files_vcl} ${src_files_third_party} ${src_files})
# 
# # Set Compiler for Unix system
# if(UNIX)
#    set(CMAKE_CXX_COMPILER g++)                      # Can switch to clang++ if prefered
#    add_definitions(-g -O2 -std=c++14 -Wall -Wextra) # Can adapt compiler flags if needed
# endif()
# 
# # Set Compiler for Windows/Visual Studio
# if(MSVC)
#     add_definitions(/MP /wd4244)   # Parallel build (/MP)
#     source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${src_files})  #Allow to explore source directories as a tree in Visual Studio
# endif()
# 
# 
# 
# # Link options for Unix
# target_link_libraries(${executable_name} ${GLFW_LIBRARIES})
# if(UNIX)
#    target_link_libraries(${executable_name} dl) #dlopen is required by Glad on Unix
# endif()
# 
