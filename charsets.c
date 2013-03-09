#include "charsets.h"
#include "convert.h"

void charset_load_ascii(character_t** characters, size_t* characters_size) {
	// add ascii chars
	const uint8_t start = 0x20; // space
	const uint8_t end = 0x7E; // ~

	for (char ascii = start; ascii <= end; ++ascii) {
		uint32_t ucs4;
		if (convert_string("UCS-4LE", "ASCII", &ascii, sizeof(ascii), (char*)&ucs4, sizeof(ucs4)) != -1)
			character_add(characters, characters_size, ucs4);
	}
}

void charset_load_iso8859_1(character_t** characters, size_t* characters_size) {
	const uint8_t start = 0xA0;

	for (uint8_t iso8859_1 = start; iso8859_1 >= start; ++iso8859_1) {
		uint32_t ucs4;
		if (convert_string("UCS-4LE", "ISO-8859-1", (char*)&iso8859_1, sizeof(iso8859_1), (char*)&ucs4, sizeof(ucs4)) != -1)
			character_add(characters, characters_size, ucs4);
	}
}

void charset_load_iso8859_7(character_t** characters, size_t* characters_size) {
	const uint8_t start = 0xA0;
	const uint8_t end = 0xFE;

	for (uint8_t iso8859_7 = start; (iso8859_7 <= end) && (iso8859_7 >= start); ++iso8859_7) {
		if ((iso8859_7 == 0xAE) || (iso8859_7 == 0xD2))
			continue;

		uint32_t ucs4;
		if (convert_string("UCS-4LE", "ISO-8859-7", (char*)&iso8859_7, sizeof(iso8859_7), (char*)&ucs4, sizeof(ucs4)) != -1)
			character_add(characters, characters_size, ucs4);
	}
}