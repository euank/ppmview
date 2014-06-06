/**
 * @file glHelper.h
 * @brief Header for OpenGL 2D image display utility functions
 *
 * @author EuanK <euank@euank.com>
 */
#ifndef GL_HELPER_H
#define GL_HELPER_H
#include <GL/gl.h>
#include <stdbool.h>
#include "ppm.h"

/**
 * Contains state for glHelper such as optional transforms,
 * the exit callback, and the image information
 */
typedef struct {
  unsigned int texture; /** OpenGL texture id */
  ppm_t *img; /** Image to be loaded into a texture and displayed */
  bool invert; /** Should the image be inverted top-to-bottom */
  bool red; /** Should the red channel be displayed */
  bool green; /** Should the green channel be displayed */
  bool blue; /** Should the blue channel be displayed */
  void (*exit)(void); /** Exit callback from caller */
} glHelperState_t;

/* This global variable (thanks opengl) keeps track of all state.
 * I opted for a single namespaced (glHelper) global struct to
 * minimize the well understood issues with global variables.
 */
glHelperState_t glHelperState;

/* Externally useful functions */
void glHelperInit(int argc, char **argv);
int glDisplayPpm(ppm_t *img, void (*exitFunc)());

/* Internally useful functions */
void glHelperLoadTexture(ppm_t *img);
void glHelperMouseHandler(int button, int state, int x, int y);
void glHelperKeypressHandler(unsigned char key, int x, int y);
void glHelperDrawLoop();
void glHelperLoadTexture(ppm_t *img);

#endif
