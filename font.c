#include <string.h>
#include "font.h"

void font_load(const FT_Library *library, font_t *font, const char* filename) {
	// open font
	FT_Error error = FT_New_Face(*library, filename, 0, &(font->ft_face));
	if (error) {
    	fprintf(stderr, "Could not open font file '%s'! (Error %u)\n", filename, error);
    	exit(EXIT_FAILURE);
	}

	// check if font has unicode support
	int unicode_support = 0;
	for (unsigned int charmap = 0; charmap < font->ft_face->num_charmaps; ++charmap) {
		if (font->ft_face->charmaps[charmap]->encoding == FT_ENCODING_UNICODE)
			unicode_support += 1;
	}
	if (!unicode_support) {
		fprintf(stderr, "Provided font has no Unicode support!\n");
		exit(EXIT_FAILURE);
	}

	font->family_name = strdup(font->ft_face->family_name);
	font->style_name = strdup(font->ft_face->style_name);

	font->italic = font->ft_face->style_flags & FT_STYLE_FLAG_ITALIC ? 1 : 0;
	font->bold = font->ft_face->style_flags & FT_STYLE_FLAG_BOLD ? 1 : 0;
	font->monospace = font->ft_face->face_flags & FT_FACE_FLAG_FIXED_WIDTH ? 1 : 0;
}

void font_set_size(font_t *font, int width, int height) {
	// set font size
	FT_Error error = FT_Set_Pixel_Sizes(font->ft_face, width, height);
	if (error) {
		fprintf(stderr, "Could not set font size to %ux%u!\n", width, height);
		exit(EXIT_FAILURE);
	}

	font->width = width;
	font->height = height;

	font->ascender = font->ft_face->size->metrics.ascender/64;
	font->descender = font->ft_face->size->metrics.descender/64;
	font->max_advance = font->ft_face->size->metrics.max_advance/64;
}

void free_font(font_t *font) {
	free(font->style_name);
	free(font->family_name);
	FT_Done_Face(font->ft_face);
}
