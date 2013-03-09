#ifndef _CHARACTER_H_
#define _CHARACTER_H_

#include <inttypes.h>
#include <stdlib.h>

typedef struct {
	// UCS-4 representation
	uint32_t ucs4;

	// position of corresponding glyph in glyphs array
	size_t glyph;
} character_t;

size_t character_add(character_t** characters, size_t* characters_size, uint32_t ucs4);
size_t character_get_position(character_t* characters, size_t characters_size, uint32_t ucs4);
void free_characters(character_t** characters, size_t* characters_size);

#endif