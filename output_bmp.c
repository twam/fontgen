#include "output_bmp.h"
#include <stdio.h>
#include "output.h"
#include "character.h"

static uint32_t max_uint32_t(uint32_t a, uint32_t b) {
	return a > b ? a : b;
}

static int output_bmp_write_header(FILE *fd, uint32_t width, uint32_t height, uint32_t pitch);

int output_bmp_write(const char* output_directory, const font_t* font, const character_t* characters, size_t characters_size, const glyph_t** glyphs, size_t glyphs_size) {
	int ret = 0;

	// Check if directory exists
	ret += output_check_directory(output_directory);
	if (ret != 0)
		return ret;

	const uint32_t characters_per_line = 16;

	uint32_t ucs4_min = characters[0].ucs4;
	uint32_t ucs4_max = characters[characters_size-1].ucs4;
	uint32_t dimension = max_uint32_t(font->height, font->width);

	uint32_t ucs4_start = (ucs4_min/characters_per_line)*characters_per_line;
	uint32_t ucs4_end = ((ucs4_max/characters_per_line)+1)*characters_per_line;

	uint32_t lines = (ucs4_end-ucs4_start)/characters_per_line;

	char* filename;
	if (asprintf(&filename, "%s/%s_%s_%i.bmp", output_directory, font->family_name, font->style_name, font->height) == -1) {
		fprintf(stderr, "Not enough memory for asprintf!\n");
		return -1;
	};

	uint32_t width = characters_per_line*dimension;
	uint32_t height = lines*dimension;
	uint32_t pitch = (width+31)/32;

	uint32_t pixeldata[height*pitch];

	FILE *fd = fopen(filename, "w");
	if (fd == NULL) {
		fprintf(stderr, "Could not open file '%s'!\n", filename);
		return -1;
	}
	free(filename);

	output_bmp_write_header(fd, width, height, pitch);

	memset(pixeldata, 0x00, sizeof(uint32_t)*pitch*height);

	for (int y = height-1; y >= 0; --y) {
		uint32_t top_y = (height-1)-y;
		uint32_t char_y = top_y/dimension;

		for (int x = 0; x < width; ++x) {
			// char

			uint32_t char_x = x/dimension;
			uint32_t char_ucs4 = ucs4_start + char_y*characters_per_line + char_x;

			// get char position in array
			size_t char_pos = character_get_position(characters, characters_size, char_ucs4);

			// do we render this char?
			if (char_pos == -1)
				continue;

			// coordinates inside char
			int32_t inside_x = x-char_x*dimension-characters[char_pos].glyph->bitmap_left;
			int32_t inside_y = top_y-char_y*dimension-(font->ascender)+characters[char_pos].glyph->bitmap_top;



			if ((inside_y < 0) || (inside_y >= characters[char_pos].glyph->bitmap_rows))
				continue;

			if ((inside_x < 0) || (inside_x >= characters[char_pos].glyph->bitmap_width))
				continue;

	 		unsigned int value = characters[char_pos].glyph->bitmap_data[inside_y*characters[char_pos].glyph->bitmap_pitch+inside_x/8] & (0x80 >> (inside_x % 8));

			if (value) {
				pixeldata[y*pitch+x/32] |= (0x80000000>>(x%32));
			}
		}
		//printf("\n");
	}

	// change endianess of pixeldata
	for (size_t i = 0; i < height*pitch; ++i) {
		uint32_t tmp = pixeldata[i];
		pixeldata[i] = (tmp << 24) | ((tmp <<  8) & 0x00ff0000) | ((tmp >>  8) & 0x0000ff00) | ((tmp >> 24) & 0x000000ff);
	}


	fwrite(pixeldata, height*pitch, sizeof(uint32_t), fd);

	fclose(fd);

	return ret;
}

static int output_bmp_write_header(FILE *fd, uint32_t width, uint32_t height, uint32_t pitch) {
	uint8_t bmp_header[14];
	uint8_t dib_header[40];
	uint8_t color_table[8];

	uint32_t imagesize = height*pitch*sizeof(uint32_t);
	uint32_t offset = sizeof(bmp_header)+sizeof(dib_header)+sizeof(color_table);
	uint32_t filesize = imagesize + offset;

	// BM string
	bmp_header[0] = 0x42;
	bmp_header[1] = 0x4D;

	// filesize in bytes
	bmp_header[2] = (filesize >> 8*0);
	bmp_header[3] = (filesize >> 8*1);
	bmp_header[4] = (filesize >> 8*2);
	bmp_header[5] = (filesize >> 8*3);

	// reserved
	bmp_header[6] = 0x00;
	bmp_header[7] = 0x00;
	bmp_header[8] = 0x00;
	bmp_header[9] = 0x00;

	// offset
	bmp_header[10] = (offset >> 8*0);
	bmp_header[11] = (offset >> 8*1);
	bmp_header[12] = (offset >> 8*2);
	bmp_header[13] = (offset >> 8*3);

	fwrite(bmp_header, 1, sizeof(bmp_header), fd);

	// DIB header size
	dib_header[0] = (sizeof(dib_header) >> 8*0);
	dib_header[1] = (sizeof(dib_header) >> 8*1);
	dib_header[2] = (sizeof(dib_header) >> 8*2);
	dib_header[3] = (sizeof(dib_header) >> 8*3);

	// width in pixels
	dib_header[4] = (width >> 8*0);
	dib_header[5] = (width >> 8*1);
	dib_header[6] = (width >> 8*2);
	dib_header[7] = (width >> 8*3);

	// height in pixels
	dib_header[8] = (height >> 8*0);
	dib_header[9] = (height >> 8*1);
	dib_header[10] = (height >> 8*2);
	dib_header[11] = (height >> 8*3);

	// color planes
	dib_header[12] = 0x01;
	dib_header[13] = 0x00;

	// number of bits per pixel
	dib_header[14] = 0x01;
	dib_header[15] = 0x00;

	// compression method
	dib_header[16] = 0x00;
	dib_header[17] = 0x00;
	dib_header[18] = 0x00;
	dib_header[19] = 0x00;

	// image size (only data)
	dib_header[20] = (imagesize >> 8*0);
	dib_header[21] = (imagesize >> 8*1);
	dib_header[22] = (imagesize >> 8*2);
	dib_header[23] = (imagesize >> 8*3);

	// horizontal dpi
	dib_header[24] = 0x12;
	dib_header[25] = 0x0B;
	dib_header[26] = 0x00;
	dib_header[27] = 0x00;

	// vertical dpi
	dib_header[28] = 0x12;
	dib_header[29] = 0x0B;
	dib_header[30] = 0x00;
	dib_header[31] = 0x00;

	// number of colors in palette
	dib_header[32] = 0x00;
	dib_header[33] = 0x00;
	dib_header[34] = 0x00;
	dib_header[35] = 0x00;

	// important colors
	dib_header[36] = 0x00;
	dib_header[37] = 0x00;
	dib_header[38] = 0x00;
	dib_header[39] = 0x00;

	fwrite(dib_header, 1, sizeof(dib_header), fd);

	color_table[0] = 0xFF;
	color_table[1] = 0xFF;
	color_table[2] = 0xFF;
	color_table[3] = 0x00;

	color_table[4] = 0x00;
	color_table[5] = 0x00;
	color_table[6] = 0x00;
	color_table[7] = 0x00;

	fwrite(color_table, 1, sizeof(color_table), fd);

	return 0;
}
