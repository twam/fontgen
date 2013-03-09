#ifndef _FONT_H_
#define _FONT_H_

#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
	char* family_name;
	char* style_name;

	int italic;
	int bold;
	int monospace;

	int width;
	int height;

	// freetype font face
	FT_Face ft_face;
} font_t;

void font_load(const FT_Library *library, font_t *font, const char* filename);
void font_set_size(font_t *font, int width, int height);
void free_font(font_t *font);

#endif