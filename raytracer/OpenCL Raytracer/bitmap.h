// bitmap.h
// bitmap writing to write output frame

#ifndef __BITMAP_H__
#define __BITMAP_H__

#include <stdint.h>
#include "common.h"

#define PIXEL_BIT_DEPTH 24
#define BITMAP_HEADER_SIZE 14
#define BITMAP_INFO_HEADER_SIZE 40

typedef struct bmp_magic {
	unsigned char magic[2];
} bmp_magic_t;

typedef struct {
  uint32_t filesz;
  uint16_t creator1;
  uint16_t creator2;
  uint32_t bmp_offset;
} BMP_HEADER;

typedef struct {
  uint32_t header_sz;
  int32_t width;
  int32_t height;
  uint16_t nplanes;
  uint16_t bitspp;
  uint32_t compress_type;
  uint32_t bmp_bytesz;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors;
  uint32_t nimpcolors;
} BMP_INFO_HEADER;

// function prototypes
int write_bmp_file(Pixel * pixels, int width, int height, const char * filename);

#endif