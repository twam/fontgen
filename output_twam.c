#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include "output_twam.h"

static int output_twam_write_headers(const char* output_directory);
static int output_twam_write_data(const char* output_directory, const font_t *font, const character_t* characters, size_t characters_size, const glyph_t* glyphs, size_t glyphs_size);
static int output_twam_write_font(FILE *fd, const font_t *font, const char* fontname_lower, size_t characters_size);
static int output_twam_write_characters(FILE *fd, const character_t* characters, size_t characters_size, const glyph_t* glyphs, const char* fontname_lower);
static int output_twam_write_glyphs(FILE *fd, const glyph_t* glyphs, size_t glyphs_size, const char* fontname_lower);

int output_twam_write(const char* output_directory, const font_t* font, const character_t* characters, size_t characters_size, const glyph_t* glyphs, size_t glyphs_size) {
	int ret = 0;

	// Check if directory exists

	if (0 != access(output_directory, R_OK | W_OK)) {
		switch (errno) {
			case EACCES:
				fprintf(stderr, "Read/Write access denied to '%s'!\n", output_directory);
				break;

			case ELOOP:
				fprintf(stderr, "Too many symbolic links were encountered while resolving '%s'!\n", output_directory);
				break;

			case ENAMETOOLONG:
				fprintf(stderr, "Path '%s' is too long!\n", output_directory);
				break;

			case ENOENT:
				fprintf(stderr, "Directory '%s' does not exist!\n", output_directory);
				break;

			case ENOTDIR:
				fprintf(stderr, "'%s' is not a directory!!\n", output_directory);
				break;

			case EROFS:
				fprintf(stderr, "'%s' is on a read-only filesystem!\n", output_directory);
				break;

			default:
				perror("access");
				break;
		}

		return -1;
	}


	// font-info
	ret += output_twam_write_headers(output_directory);
	ret += output_twam_write_data(output_directory, font, characters, characters_size, glyphs, glyphs_size);

	return ret;
}

static int output_twam_write_data(const char* output_directory, const font_t *font, const character_t* characters, size_t characters_size, const glyph_t* glyphs, size_t glyphs_size) {
	int ret = 0;

	char* fontname;
	asprintf(&fontname, "%s_%s_%i", font->family_name, font->style_name, font->height);

	char* fontname_lower = strdup(fontname);
	char* fontname_upper = strdup(fontname);
	for (size_t i = 0; i < strlen(fontname); ++i) {
		fontname_lower[i] = tolower(fontname[i]);
		fontname_upper[i] = toupper(fontname[i]);
	}

	FILE *fd;
	char* filename;

	// header file
	asprintf(&filename, "%s/%s.h", output_directory, fontname_lower);
	fd = fopen(filename, "w");
	if (fd == NULL) {
		fprintf(stderr, "Could not open file '%s'!\n", filename);
		return -1;
	}
	free(filename);

	fprintf(fd,
		"#ifndef _%s_H_\n"
		"#define _%s_H_\n"
		"#include \"font.h\"\n"
		"#include \"character.h\"\n"
		"#include \"glyph.h\"\n\n"
		"extern const font_t %s;\n\n"
		"#endif\n"
		, fontname_upper, fontname_upper, fontname_lower);

	fclose(fd);

	// c file
	asprintf(&filename, "%s/%s.c", output_directory, fontname_lower);
	fd = fopen(filename, "w");
	if (fd == NULL) {
		fprintf(stderr, "Could not open file '%s'!\n", filename);
		return -1;
	}
	free(filename);

	fprintf(fd,	"#include \"%s.h\"\n\n", fontname_lower);
	ret += output_twam_write_glyphs(fd, glyphs, glyphs_size, fontname_lower);
	ret += output_twam_write_characters(fd, characters, characters_size, glyphs, fontname_lower);
	ret += output_twam_write_font(fd, font, fontname_lower, characters_size);

	fclose(fd);

	free(fontname);
	free(fontname_lower);
	free(fontname_upper);

	return 0;
}

static int output_twam_write_font(FILE *fd, const font_t *font, const char* fontname_lower, size_t characters_size) {
	fprintf(fd,
		"const font_t %s = {\n"
		"\t\"%s\",\t// family_name\n"
		"\t\"%s\",\t// style_name\n"
		"\t%i,\t// width\n"
		"\t%i,\t// height\n"
		"\t%i,\t// italic\n"
		"\t%i,\t// bold\n"
		"\t%i,\t// monospace\n"
		"\t%s_characters,\t// characters\n"
		"\t%li\t// characters_size\n"
		"};\n"
		, fontname_lower, font->family_name, font->style_name, font->width, font->height, font->italic, font->bold, font->monospace, fontname_lower, characters_size);

	return 0;
}

static int output_twam_write_characters(FILE *fd, const character_t* characters, size_t characters_size, const glyph_t* glyphs, const char* fontname_lower) {
	fprintf(fd, "const character_t %s_characters[] = {\n", fontname_lower);

	for (size_t i = 0; i < characters_size; ++i) {
		fprintf(fd,	"\t{0x%08X, &%s_glyph_%i}, \n", characters[i].ucs4, fontname_lower, glyphs[characters[i].glyph].glyph_index);
	}

	fprintf(fd, "};\n\n");

	return 0;
}

static int output_twam_write_glyphs(FILE *fd, const glyph_t* glyphs, size_t glyphs_size, const char* fontname_lower) {
	for (size_t i = 0; i < glyphs_size; ++i) {
		fprintf(fd,
			"const glyph_t %s_glyph_%i = {\n"
			"\t%i,\t// left;\n"
			"\t%i,\t// top;\n"
			"\t%i,\t// rows;\n"
			"\t%i,\t// width;\n"
			"\t%i,\t// pitch;\n"
			"\t{\t// %s\n"
			, fontname_lower, glyphs[i].glyph_index, glyphs[i].bitmap_left, glyphs[i].bitmap_top, glyphs[i].bitmap_rows, glyphs[i].bitmap_width, glyphs[i].bitmap_pitch, glyphs[i].name);

			for (int row = 0; row <glyphs[i].bitmap_rows; ++row) {
				fprintf(fd, "\t\t");
				for (int pitch = 0; pitch < glyphs[i].bitmap_pitch; ++pitch) {
					fprintf(fd, "0x%02hhX, ", glyphs[i].bitmap_data[row*glyphs[i].bitmap_pitch+pitch]);
			 	}

			 	fprintf(fd, "// ");
				for (int col = 0; col < glyphs[i].bitmap_width; ++col) {
			 		unsigned int value = glyphs[i].bitmap_data[row*glyphs[i].bitmap_pitch+col/8] & (0x80 >> (col % 8));
			 		if (value) {
			 			fprintf(fd, "X");
			 		} else {
			 			fprintf(fd, ".");
			 		}
			 	}
			 	fprintf(fd, "\n");
			}


		fprintf(fd,
			"\t}\n"
			"};\n\n"
			);
	}

	return 0;
}

static int output_twam_write_headers(const char* output_directory) {
	FILE *fd;
	char* filename;

	// font.h
	asprintf(&filename, "%s/font.h", output_directory);
	fd = fopen(filename, "w");
	if (fd == NULL) {
		fprintf(stderr, "Could not open file '%s'!\n", filename);
		return -1;
	}
	free(filename);

	fprintf(fd,
		"#ifndef _FONT_H_\n"
		"#define _FONT_H_\n"
		"#include <character.h>\n"
		"#include <inttypes.h>\n\n"
		"typedef struct {\n"
		"\tconst char* family_name;\n"
		"\tconst char* style_name;\n"
		"\tuint8_t width;\n"
		"\tuint8_t height;\n"
		"\tuint8_t italic;\n"
		"\tuint8_t bold;\n"
		"\tuint8_t monospace;\n"
		"\tconst character_t* characters;\n"
		"\tuint32_t characters_size;\n"
		"} font_t;\n\n"
		"#endif\n"
		);

	fclose(fd);

	// character.h
	asprintf(&filename, "%s/character.h", output_directory);
	fd = fopen(filename, "w");
	if (fd == NULL) {
		fprintf(stderr, "Could not open file '%s'!\n", filename);
		return -1;
	}
	free(filename);

	fprintf(fd,
		"#ifndef _CHARACTER_H_\n"
		"#define _CHARACTER_H_\n"
		"#include <glyph.h>\n"
		"#include <inttypes.h>\n\n"
		"typedef struct {\n"
		"\tuint32_t ucs4;\n"
		"\tconst glyph_t *glyph;\n"
		"} character_t;\n\n"
		"#endif\n"
		);

	fclose(fd);


	// glyph.h
	asprintf(&filename, "%s/glyph.h", output_directory);
	fd = fopen(filename, "w");
	if (fd == NULL) {
		fprintf(stderr, "Could not open file '%s'!\n", filename);
		return -1;
	}
	free(filename);

	fprintf(fd,
		"#ifndef _GLYPH_H_\n"
		"#define _GLYPH_H_\n"
		"#include <inttypes.h>\n\n"
		"typedef struct {\n"
		"\tint8_t left;\n"
		"\tint8_t top;\n"
		"\tuint8_t rows;\n"
		"\tuint8_t width;\n"
		"\tuint8_t pitch;\n"
		"\tuint8_t data[];\n"
		"} glyph_t;\n\n"
		"#endif\n"
		);

	fclose(fd);

	return 0;
}
