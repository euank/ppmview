cmake_minimum_required(VERSION 2.6)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(ppmview)
add_subdirectory(src)

set_target_properties(ppmview PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wno-unused-parameter -g -pedantic -std=gnu99")

add_custom_target(run COMMAND ppmview cube.ppm DEPENDS ppmview WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})

add_custom_target(docs COMMAND doxygen doxygen.conf WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})
