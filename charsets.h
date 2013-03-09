#ifndef _CHARSETS_H_
#define _CHARSETS_H_

#include <stdlib.h>
#include "character.h"

void charset_load_ascii(character_t** characters, size_t* characters_size);
void charset_load_iso8859_1(character_t** characters, size_t* characters_size);
void charset_load_iso8859_7(character_t** characters, size_t* characters_size);

#endif