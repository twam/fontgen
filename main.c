#include <stdio.h>
#include <stdlib.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H

#include "options.h"
#include "character.h"
#include "convert.h"
#include "glyph.h"
#include "font.h"
#include "charsets.h"
#include "output_twam.h"
#include "output_bmp.h"

int main(int argc, char** argv) {
	FT_Library library;
	arguments_t arguments;
	size_t characters_size = 0;
	character_t* characters = malloc(sizeof(character_t)*characters_size);
	size_t glyphs_size = 0;
	glyph_t* glyphs = malloc(sizeof(glyph_t)*glyphs_size);
	font_t font;

	set_default_values(&arguments);

	parse(argc, argv, &arguments);

	// init freetype library
	FT_Error error = FT_Init_FreeType(&library);
	if (error) {
    	fprintf(stderr, "Could not initialize FreeType library! (Error %u)\n", error);
    	exit(EXIT_FAILURE);
	}

	font_load(&library, &font, arguments.font_filename);
	font_set_size(&font, arguments.font_width, arguments.font_height);

	// load character sets
	for (size_t i = 0; i < arguments.charsets_size; ++i) {
		switch (arguments.charsets[i]) {
			case ASCII:
				charset_load_ascii(&characters, &characters_size);
				break;
			case ISO_8859_1:
				charset_load_iso8859_1(&characters, &characters_size);
				break;
			case ISO_8859_7:
				charset_load_iso8859_7(&characters, &characters_size);
				break;
		}
	}

	// load single characters
	if (arguments.characters != NULL) {
		char *next = arguments.characters;
		size_t size = strlen(arguments.characters);
		while (next < arguments.characters+size) {
			uint32_t ucs4 = strtoul(next, &next, 0);
			if (ucs4 == 0) {
				fprintf(stderr, "Invalid character '%c' or character 0 while parsing characters.\n", *next);
				exit(EXIT_FAILURE);
			}

			if (*next == '-') {
				next++;
				uint32_t ucs4_end = strtoul(next, &next, 0);
				if (ucs4 > ucs4_end) {
					fprintf(stderr, "Range %u-%u is negative while parsing characters.\n", ucs4, ucs4_end);
					exit(EXIT_FAILURE);
				} else {
					for (uint32_t i = ucs4; i <= ucs4_end; ++i) {
						character_add(&characters, &characters_size, i);
					}
				}
			} else {
				character_add(&characters, &characters_size, ucs4);
			}

			while (*next == ',') {
				next++;
			}
		}
	}

	if (characters_size == 0) {
		fprintf(stderr, "You haven't loaded any characters!\n");
		exit(EXIT_FAILURE);
	}

	printf("Generated %li characters.\n", characters_size);

	glyphs_load(&glyphs, &glyphs_size, &font, characters, characters_size);

	switch (arguments.output_format) {
		case BMP:
			output_bmp_write(arguments.output_directory, &font, characters, characters_size, glyphs, glyphs_size);
			break;

		case TWAM:
			output_twam_write(arguments.output_directory, &font, characters, characters_size, glyphs, glyphs_size);
			break;
	}

	free_arguments(&arguments);
	free_glyphs(&glyphs, &glyphs_size);
	free_characters(&characters, &characters_size);
	free_font(&font);
	FT_Done_FreeType(library);

	return EXIT_SUCCESS;
}