/**
 * @file ppm.h
 * @brief Simple image structure and header for ppm reading and writing functions
 *
 * @author EuanK <euank@euank.com>
 */
#ifndef PPM_H
#define PPM_H
#include <stdint.h>

/* Originally I had a pixel struct too,
 * but I really don't want to write unportable
 * code in using __attribute__((packed)), but
 * it's absolutely necessary to guarantee the
 * array of pixels is tightly packed for opengl.
 */

/**
 * A struct representing an image
 * max_color is unused due to the limited scope of this assignment
 */
typedef struct {
  unsigned int width; /** Image width */
  unsigned int height; /** Image height */
  uint8_t *pixels; /** Tighly packed array of pixels in rgb format, row-major */
  uint8_t max_color; /* Assumed to be 255 */ /** maximum pixel value, indicating 100% color for that color */
  char *filename;  /** Images input filename */
} ppm_t;


ppm_t *loadPpm(char *filename);
int writePpm(ppm_t *ppm, char *filename);
void freePpm(ppm_t *ppm);

#endif
