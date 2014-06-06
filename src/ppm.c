/**
 * @file ppm.c
 * @brief Functions for easily reading and writing ppm files given filenames
 *
 * @author EuanK <euank@euank.com>
 */

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include "ppm.h"

/**
 * An enum indicating which ppm header section is being parsed
 * This enum is used as a state machine for going
 * through the ppm header. It was inspired by reading
 * http://netpbm.sourceforge.net/doc/ppm.html where
 * it states that each of these items must be in the
 * header in this order essentially. Because enums in c
 * are garunteed to be sequential, the section can be
 * incremented to proceed
 */
typedef enum {
  MAGIC_NUMBER,
  WHITESPACE1,
  WIDTH,
  WHITESPACE2,
  HEIGHT,
  WHITESPACE3,
  MAX_COLOR_VAL,
  WHITESPACE_CHAR,
  END
} ppm_header_section;

/**
 * Loads a ppm from a given file
 * If there is a nonfatal error it prents to stderr and attempts to continue
 * as the ppm specs indicate any tolerances possible should be made.
 * @param filename The path of the file to load, passed to fopen
 * @return A ppm struct containing the image data read. NULL on fatal error.
 */
ppm_t *loadPpm(char *filename) {
  errno = 0;
  FILE *f = fopen(filename, "r");
  if(f == NULL) {
    return NULL;
  }

  ppm_t *img = malloc(sizeof(ppm_t));
  unsigned int max_color = 0;
  char *line = NULL;
  size_t ignored;
  ssize_t lineLength;
  ssize_t lineIndex;
  ppm_header_section section = MAGIC_NUMBER;

  img->width = 0;
  img->height = 0;
  img->pixels = NULL;

  /* getline is gnu99 standard, but not ansi c. Thankfully my makefile enforces gnu99
   * It mallocs line for you appropriately */
  while(section != END && (lineLength = getline(&line, &ignored, f)) >= 0) {
    for(lineIndex=0; lineIndex < lineLength; lineIndex++) {
      if(line[lineIndex] == '#') {
        /* Comment, skip rest of line */
        break;
      }

      switch(section) {
        case MAGIC_NUMBER:
          if(lineLength < 2 || line[0] != 'P' || line[1] != '6') {
            fprintf(stderr, "Error, file should start with P6, got %s instead\n", line);
          }
          lineIndex++;
          section++;
          break;
        case WHITESPACE1:
        case WHITESPACE2:
        case WHITESPACE3:
          if(!isspace(line[lineIndex])) {
            /* We hit the end of the whitespace.
             * We rewind by 1 so the next section
             * gets its first character correctly */
            lineIndex--;
            section++;
          }
          /* We don't error on 'no whitespace' because we'd have to differentiate
           * between the first, and subsequent, whitespace chars. There's no need really. 
           */
          break;
        case WIDTH:
          if(!isdigit(line[lineIndex])) {
            /* We hit the end of the width */
            lineIndex--;
            section++;
          } else {
            img->width = 10 * img->width + line[lineIndex] - '0';
          }
          break;
        case HEIGHT:
          if(!isdigit(line[lineIndex])) {
            /* We hit the end of the height */
            lineIndex--;
            section++;
          } else {
            img->height = 10 * img->height + line[lineIndex] - '0';
          }
          break;
        case MAX_COLOR_VAL:
          if(!isdigit(line[lineIndex])) {
            /* We hit the end of the color vitness */
            lineIndex--;
            section++;
          } else {
            max_color = 10 * max_color + line[lineIndex] - '0';
          }
          break;
        case WHITESPACE_CHAR:
          if(!isspace(line[lineIndex])) {
            fprintf(stderr, "Error, expected space but didn't find one\n");
          }
          section++;
          break;
        case END:
          /* This should never happen */
          fprintf(stderr, "Unexpected end of the world\n");
          break;
      }
    }

    /* getline promises that line is free-able */
    free(line);
    line = NULL;
  }

  if(max_color != 255) {
    fprintf(stderr, "Only 'base 255' color scales are supported right now\n");
    max_color = 255;
    /* Actually probably not fatal, it'll just look bad.
     * Give the user a warning and move on */
  }

  img->max_color = max_color;

  img->pixels = malloc(img->width * img->height * 3);

  if(fread(img->pixels, 1, img->width * img->height * 3, f) != img->width * img->height * 3) {
    fprintf(stderr, "Read the wrong number of pixels..\n");
    /* Sorta fatal, but it's possible our data isn't completely trash. 
     * if it's 0 it really is, but we can still give them a (probablistically) white 
     * image and stderr to let them know something is wrong */
  }

  img->filename = strdup(filename);

  fclose(f);
  return img;
}

/**
 * Saves a ppm struct to file as a valid ppm image
 * @param img The ppm struct representing the image to save
 * @param filename The filename to save it as which will be passed to fopen
 * @returns 0 on success, a relevant errno on any sort of problem.
 */
int writePpm(ppm_t *img, char *filename) {
  errno = 0;
  FILE *f = fopen(filename, "w");
  if(f == NULL) {
    return errno;
  }
  fprintf(f, "P6\n#Created with ppmview: euank@euank.com\n%d %d\n%d\n", img->width, img->height, 255);

  if(fwrite(img->pixels, 1, img->width * img->height * 3, f) != img->width * img->height * 3) {
    fprintf(stderr, "Error writing new image!\n");
  }
  fclose(f);
  return errno;
}

/**
 * Frees all memory used by a ppm struct
 * @param img A ppm struct that has been previously allocated
 */
void freePpm(ppm_t *img) {
  free(img->pixels);
  free(img->filename);
  free(img);
}
