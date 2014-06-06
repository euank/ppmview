/**
 * @file glHelper.c
 * @brief Functions enabling simple display of a 2D image through glTextures
 *
 * @author EuanK <euank@euank.com>
 * @bugs Image displays inverted on the samsung chromebook SoC with freeglut 1.2.
 * Reason unknown
 */

#define GL_GLEXT_PROTOTYPES 1
#include <GL/glut.h>
#include <GL/gl.h>
#include <stdio.h> /* snprintf */

#include "glHelper.h"
#include "ppm.h"

/* 255 chars for filename, 11 for " - ppmview\0". 255 chosen for
 * historical and irrelevant reasons */
#define GL_HELPER_MAX_WINTITLE_LENGTH 266

/**
 * Inititalizes glut and the global glHelper struct
 * @param argc Number of args from main
 * @param argv Args from main
 */
void glHelperInit(int argc, char **argv) {
  glutInit(&argc, argv);
  glHelperState.invert = false;
  glHelperState.red = glHelperState.green = glHelperState.blue = true;
  glHelperState.exit = NULL;
}

/**
 * Loads the ppm texture into opengl
 * At this point the ppm_t could be freed, it's no longer needed.
 * This also enables the few required options to use a texture
 * @param img Image to load into the opengl texture
 */
void glHelperLoadTexture(ppm_t *img) {
  glEnable(GL_TEXTURE_2D);

  glGenTextures(1, &glHelperState.texture);
  glBindTexture(GL_TEXTURE_2D, glHelperState.texture);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  /* Because I have tighly packed rows, the following is required for images that
   * have non-multiple-of-4 widths to display correctly. Thanks opengl
   */
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0,
      GL_RGB, img->width, img->height, 0, GL_RGB,
      GL_UNSIGNED_BYTE, img->pixels);
}

/**
 * The opengl draw function which draws the texture onto a rectangle
 */
void glHelperDrawLoop() {
  int textureTop, textureBottom;

  /* Flip the texture-top and texture-bottom to invert it, no need
   * to fiddle with pixels */
  if(glHelperState.invert) {
    textureTop = 1, textureBottom = 0;
  } else {
    textureTop = 0, textureBottom = 1;
  }

  /* Set all colors on so the background is properly colored. If you don't do this,
   * artifacts remain when resizing the window, among other things. In fact, with a
   * transparent window you'll have artifacts from the background follow you as you 
   * move it around. Looked kind of trippy, but obviously wrong */
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* Enable color masks if desired */
  glColorMask(glHelperState.red ? GL_TRUE : GL_FALSE, 
              glHelperState.green ? GL_TRUE : GL_FALSE,
              glHelperState.blue ? GL_TRUE : GL_FALSE,
              GL_TRUE); /* Alpha doesn't matter */

  glBegin(GL_QUADS);
    glTexCoord2i(0, textureBottom);
    glVertex2i(-1, -1);

    glTexCoord2i(0, textureTop);
    glVertex2i(-1, 1);

    glTexCoord2i(1, textureTop);
    glVertex2i(1, 1);

    glTexCoord2i(1, textureBottom);
    glVertex2i(1, -1);
  glEnd();
  glFlush();
}

/**
 * The glut keyboard function
 * This function allows q to quit, i to invert, rgb to each apply masks, and o to 
 * remove all masks. It redisplays after any keyboard input which definitely changed the image.
 * To do this it stores a copy of the state before and checks if it differs after.
 * @param key Key that was pressed 
 * @param x Mouse x coord in the window
 * @param y Mouse y coord in the window
 */
void glHelperKeypressHandler(unsigned char key, int x, int y) {
  glHelperState_t oldState = glHelperState;
  switch(key) {
    case 'q':
    case 'Q':
      if(glHelperState.exit != NULL) {
        glHelperState.exit();
      }
      exit(0);
    case 'i':
    case 'I':
      glHelperState.invert = !glHelperState.invert;
      glutPostRedisplay();
      break;
    case 'r':
    case 'R':
      glHelperState.red = 1;
      glHelperState.green = 0;
      glHelperState.blue = 0;
      glutPostRedisplay();
      break;
    case 'g':
    case 'G':
      glHelperState.red = 0;
      glHelperState.green = 1;
      glHelperState.blue = 0;
      glutPostRedisplay();
      break;
    case 'b':
    case 'B':
      glHelperState.red = 0;
      glHelperState.green = 0;
      glHelperState.blue = 1;
      glutPostRedisplay();
      break;
    case 'o':
    case 'O':
      glHelperState.red = 1;
      glHelperState.green = 1;
      glHelperState.blue = 1;
      glutPostRedisplay();
      break;
  }
  /* See if anything changed that requires a redraw.. */
  if(oldState.invert != glHelperState.invert ||
      oldState.red != glHelperState.red ||
      oldState.green != glHelperState.green ||
      oldState.blue != glHelperState.blue) {
    glutPostRedisplay();
  }
}

/**
 * Glut mouse function
 * This function quits on any mouse activity. It first calls the exit callback, if one was given.
 * @param button Which mouse button
 * @param state If it was a mouseup or mousedown event
 * @param x The x position of the mouse within the window
 * @param y The y position of the mouse within the window
 */
void glHelperMouseHandler(int button, int state, int x, int y) {
  if(glHelperState.exit != NULL) {
    glHelperState.exit();
  }
  exit(0);
}

/**
 * Creates a window showing the passed in image
 * The window title is set to the image name and the image is initialized to the right size.
 * The position is intentionally not manipulated because that's the job of the window manager
 * and I personally dislike programs hijacking windows starting location.
 * @param img The image to display
 * @param exitFunc A callback to call on exit
 * @return N/A, this function never returns
 */
int glDisplayPpm(ppm_t *img, void (*exitFunc)()) {
  glHelperState.img = img;
  glHelperState.exit = exitFunc;

  glutInitWindowSize(img->width, img->height);

  char *winTitle = malloc(GL_HELPER_MAX_WINTITLE_LENGTH);
  snprintf(winTitle, GL_HELPER_MAX_WINTITLE_LENGTH, "%s - ppmview", img->filename);
  glutCreateWindow(winTitle);

  free(winTitle);

  glHelperLoadTexture(img);

  glutDisplayFunc(glHelperDrawLoop);
  glutKeyboardFunc(glHelperKeypressHandler);
  glutMouseFunc(glHelperMouseHandler);

  glutMainLoop();
  return 0;
}
