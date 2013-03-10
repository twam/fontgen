#ifndef _OUTPUT_TWAM_H_
#define _OUTPUT_TWAM_H_

#include <stdlib.h>
#include "font.h"
#include "character.h"
#include "glyph.h"

int output_twam_write(const char* output_directory, const font_t* font, const character_t* characters, size_t characters_size, const glyph_t** glyphs, size_t glyphs_size);

#endif