/**
 * @file ppmview.c
 * @brief The program entry point. Loads an image, optionally writes a duplicate, and kicks off displaying it.
 * 
 * @author EuanK <euank@euank.com>
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h> /* strerror */

#include "ppm.h"
#include "glHelper.h"

/* Global image so that it can be freed on exit. This wouldn't be necessary if
 * you could pass control back from glutMainLoop, but it turns out the loop structure
 * used is for( ;; ), so no chance of doing so without monkeypatching glut... not a
 * good idea */
ppm_t *ppmviewImg;

/**
 * An exit callback to run before quitting
 */
void onExit() {
  /* glHelper calls this before exiting assuming it exited nicely enough */
  freePpm(ppmviewImg);
}

/**
 * The main function
 * Checks args, does appropriate file-related actions, and passes it on to opengl
 * @param argc number of args
 * @param argv Args
 * @return N/A, opengl's inifinite loop means this never returns.
 */
int main(int argc, char **argv) {
  /* Only support 1 or 2 arguments */
  if(argc < 2 || argc > 3) {
    fprintf(stderr, "Usage:\n\t%s <in.ppm> [<out.ppm>]\n", argv[0]);
    return 1;
  }

  /* Load file given in first argument */
  ppmviewImg = loadPpm(argv[1]);
  if(ppmviewImg == NULL) {
    /* File probably didn't exist, print out more detailed error */
    fprintf(stderr, "Error loading %s: %d - %s\n", argv[1], errno, strerror(errno));
    return errno; /* Fatal */
  }

  /* writePpm returns the errno on error, so this test works */
  if(argc == 3 && writePpm(ppmviewImg, argv[2]) > 0) {
    fprintf(stderr, "Error writing %s: %d - %s\n", argv[2], errno, strerror(errno));
    /* Non-fatal, we can still display. No need to quit */
  }

  /* Pass it off to glHelper */
  glHelperInit(argc, argv);
  glDisplayPpm(ppmviewImg, onExit);

  return 0; /* Unreachable code ;_; */
}
