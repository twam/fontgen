#include "glyph.h"
#include "character.h"

glyph_t* glyph_add(glyph_t*** glyphs, size_t* glyphs_size, FT_Face face, FT_UInt glyph_index) {
	glyph_t* glyph = glyph_get(*glyphs, *glyphs_size, glyph_index);
	// skip adding if we already have this character
	if (glyph != NULL) {
		return glyph;
	}

	// resize characters array
	*glyphs_size += 1;
	*glyphs = realloc(*glyphs, sizeof(glyph_t*)*(*glyphs_size));

	// get correct position for insertion
	size_t pos = 0;
	while ((pos+1 < *glyphs_size) && ((*glyphs)[pos]->glyph_index < glyph_index))
		pos++;

	// move everything after character
	size_t pos2 = *glyphs_size-1;
	while (pos2 > pos) {
		(*glyphs)[pos2] = (*glyphs)[pos2-1];
		pos2--;
	}

	// set-up new character
	(*glyphs)[pos] = malloc(sizeof(glyph_t));
	glyph = (*glyphs)[pos];

	glyph->glyph_index = glyph_index;

	// setup name
	if (FT_HAS_GLYPH_NAMES(face)) {
		char buffer[100];

		FT_Get_Glyph_Name(face, glyph_index, buffer, sizeof(buffer));

		glyph->name = strdup(buffer);
	} else {
		if (asprintf(&(glyph->name), "glyph_%u", glyph_index) == -1) {
			fprintf(stderr, "Not enough memory for asprintf!\n");
			glyph->name = NULL;
		}
	}

	// load glyph
	FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);

	// render glyph
	FT_Render_Glyph(face->glyph, FT_RENDER_MODE_MONO);

	glyph->advance = face->glyph->linearHoriAdvance/0x10000L;
	glyph->bitmap_left = face->glyph->bitmap_left;
	glyph->bitmap_top = face->glyph->bitmap_top;

	if (face->glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO) {
		glyph->bitmap_width = face->glyph->bitmap.width;
		glyph->bitmap_rows = face->glyph->bitmap.rows;
		glyph->bitmap_pitch = (glyph->bitmap_width)/8+1;

		glyph->bitmap_data = malloc(sizeof(unsigned char*)*glyph->bitmap_rows*glyph->bitmap_pitch);

		// pixel data is stored MSB in face->glyph->bitmap.buffer but we want LSB
		for (int row = 0; row < glyph->bitmap_rows; ++row) {
			for (int pitch = 0; pitch < glyph->bitmap_pitch; ++pitch) {
				glyph->bitmap_data[row*glyph->bitmap_pitch+pitch] = face->glyph->bitmap.buffer[row*face->glyph->bitmap.pitch+pitch];
			}
		}
	}

	return glyph;
}

glyph_t* glyph_get(glyph_t** glyphs, size_t glyphs_size, FT_UInt glyph_index) {
	for (size_t i = 0; i < glyphs_size; ++i) {
		if (glyphs[i]->glyph_index == glyph_index) {
			return glyphs[i];
		}
	}

	return NULL;
}

void glyphs_load(glyph_t*** glyphs, size_t* glyphs_size, font_t* font, character_t* characters, size_t characters_size) {
	for (size_t i = 0; i < characters_size; ++i) {
		// get corresponding glyph index from freetype
		FT_UInt glyph_index = FT_Get_Char_Index(font->ft_face, characters[i].ucs4);

		// add glyph to glyphs array
		glyph_t* glyph = glyph_add(glyphs, glyphs_size, font->ft_face, glyph_index);

		// save glyph position in character information
		characters[i].glyph = glyph;
	}
}

void free_glyphs(glyph_t*** glyphs, size_t* glyphs_size) {
	for (size_t i = 0; i < *glyphs_size; ++i) {
		free ((*glyphs)[i]->name);
		free ((*glyphs)[i]->bitmap_data);
		free ((*glyphs)[i]);
	}

	free(*glyphs);
	glyphs = NULL;
	glyphs_size = 0;
}
