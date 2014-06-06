find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
add_executable(ppmview ppmview.c ppm.c ppm.h glHelper.h glHelper.c)


target_link_libraries(ppmview ${OPENGL_LIBRARIES} ${GLUT_LIBRARY})

