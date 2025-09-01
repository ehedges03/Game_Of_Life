
# TODO: use set() to name specific sources once this project is stable
file(GLOB_RECURSE src_files ${PROJECT_SOURCE_DIR}/src/*.[ch]pp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch])
file(GLOB_RECURSE test_srcs ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch]pp ${CMAKE_CURRENT_SOURCE_DIR}/src/*.[ch])

set(TEST_NAME "Test${PROJECT_NAME}")

add_executable(${TEST_NAME} ${src_files} ${test_srcs})

# Link with FunniLib
target_link_libraries(${TEST_NAME} PRIVATE LibFunni PRIVATE glfw PRIVATE glad::glad)
