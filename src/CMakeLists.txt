# TODO: use set() to name specific sources once this project is stable
file(GLOB_RECURSE src_files ${CMAKE_CURRENT_SOURCE_DIR}/*.[ch]pp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch])

add_executable(${PROJECT_NAME} ${src_files})

# Link with FunniLib
target_link_libraries(${PROJECT_NAME} PRIVATE LibFunni PRIVATE glfw PRIVATE glad::glad)
