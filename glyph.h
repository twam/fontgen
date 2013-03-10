#ifndef _GLYPH_H_
#define _GLYPH_H_

#include <ft2build.h>
#include FT_FREETYPE_H
#include "font.h"
#include "character.h"

typedef struct {
	// glyph name
	char* name;

	///
	int advance;

	/// This is the bitmap's left bearing expressed in integer pixels
	int bitmap_left;
	/// This is the bitmap's top bearing expressed in integer pixels. Remember that this is the distance from the baseline to the top-most glyph scanline, upwards y coordinates being positive.
	int bitmap_top;

	/// The number of bitmap rows.
	int bitmap_rows;
	/// The number of pixels in bitmap row.
	int bitmap_width;
	/// The pitch's absolute value is the number of bytes taken by one bitmap row, including padding. However, the pitch is positive when the bitmap has a ‘down’ flow, and negative when it has an ‘up’ flow.
	int bitmap_pitch;

	/// Actual bitmap data
	unsigned char* bitmap_data;

	// freetype glyph index
	FT_UInt glyph_index;
} glyph_t;

size_t glyph_add(glyph_t** glyphs, size_t* glyphs_size, FT_Face, FT_UInt glyph_index);
size_t glyph_get_position(glyph_t* glyphs, size_t glyphs_size, FT_UInt glyph_index);
void glyphs_load(glyph_t** glyphs, size_t* glyphs_size, font_t* font, character_t* characters, size_t characters_size);
void free_glyphs(glyph_t** glyphs, size_t* glyphs_size);

#endif