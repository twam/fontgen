#include <stdlib.h>
#include "character.h"

size_t character_add(character_t** characters, size_t* characters_size, uint32_t ucs4) {
	// skip adding if we already have this character
	if (character_get_position(*characters, *characters_size, ucs4) != -1)
		return -1;

	// resize characters array
	*characters_size += 1;
	*characters = realloc(*characters, sizeof(character_t)*(*characters_size));

	// get correct position for insertion
	size_t pos = 0;
	while ((pos+1 < *characters_size) && ((*characters)[pos].ucs4 < ucs4))
		pos++;

	// move everything after character
	size_t pos2 = *characters_size-1;
	while (pos2 > pos) {
		(*characters)[pos2] = (*characters)[pos2-1];
		pos2--;
	}

	// set-up new character
	(*characters)[pos].ucs4 = ucs4;

	return pos;
}

size_t character_get_position(const character_t* characters, size_t characters_size, uint32_t ucs4) {
	for (size_t i = 0; i < characters_size; ++i) {
		if (characters[i].ucs4 == ucs4) {
			return i;
		}
	}

	return -1;
}

void free_characters(character_t** characters, size_t* characters_size) {
	free(*characters);
	*characters = NULL;
	*characters_size = 0;
}