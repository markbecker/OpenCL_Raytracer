// bitmap.c
// bitmap writing for output frame

#include <stdio.h>
#include "bitmap.h"
#include "common.h"

int write_bmp_file(Pixel * pixels, int width, int height, const char * filename)
{
	// some calculations for bitmap size
	int row_size = (PIXEL_BIT_DEPTH / 8) * width;
	int row_size_padding = (row_size % 4) == 0 ? 0 : 4 - (row_size % 4);
	int pixel_array_size = (row_size + row_size_padding) * height;
	int file_size = BITMAP_HEADER_SIZE + BITMAP_INFO_HEADER_SIZE + pixel_array_size;

	// magic bytes for standard bitmap
	bmp_magic_t magic_bytes;
	magic_bytes.magic[0] = 'B';
	magic_bytes.magic[1] = 'M';
	// bitmap header
	BMP_HEADER bitmap_header;
	bitmap_header.filesz = file_size;
	bitmap_header.creator1 = 0; // not used
	bitmap_header.creator2 = 0; // not used
	bitmap_header.bmp_offset = BITMAP_HEADER_SIZE + BITMAP_INFO_HEADER_SIZE;
	// bitmap info header
	BMP_INFO_HEADER bitmap_info_header;
	bitmap_info_header.header_sz = BITMAP_INFO_HEADER_SIZE;
	bitmap_info_header.width = width;
	bitmap_info_header.height = height;
	bitmap_info_header.nplanes = 1; // number of color planes
	bitmap_info_header.bitspp = 24; // number of bits per pixel. 0-7 = b, 8-15 = g, 16-23 = r
	bitmap_info_header.compress_type = 0; // no compression
	bitmap_info_header.bmp_bytesz = pixel_array_size;
	bitmap_info_header.hres = 2835; // 2835 pixels/meter = 72 dpi of most monitors
	bitmap_info_header.vres = 2835; 
	bitmap_info_header.ncolors = 0; // we have no palette
	bitmap_info_header.nimpcolors = 0; // 0 = all colors are important

	// write file
	FILE * bitmap_file;
	bitmap_file = fopen(filename, "wb");
	if (bitmap_file == NULL)
	{
		fprintf(stderr, "Error opening file %s for writing\r\n", filename);
		return 1;
	}
	// write headers
	fwrite(&magic_bytes, sizeof magic_bytes, 1, bitmap_file);
	fwrite(&bitmap_header, sizeof bitmap_header, 1, bitmap_file);
	fwrite(&bitmap_info_header, sizeof bitmap_info_header, 1, bitmap_file);

	// write pixels
	// note that in standard bitmap format, the pixels at the bottom of the image
	// are written first, but our internal structure has those pictures at max
	// height
	// therefore, we loop backwards
	for (int i = height - 1; i > -1; i--)
	{
		for (int j = 0; j < width; j++)
		{
			int cur_pixel = i * width + j;
			// pixel = cl_char4
			// r = s[0] g = s[1] b = s[2]
			fputc(pixels[cur_pixel].s[2], bitmap_file);
			fputc(pixels[cur_pixel].s[1], bitmap_file);
			fputc(pixels[cur_pixel].s[0], bitmap_file);
		}
		for (int r = 0; r < row_size_padding; r++)
			fputc(0, bitmap_file);
	}

	fclose(bitmap_file);
	return 0;
}