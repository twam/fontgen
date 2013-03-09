#ifndef _OPTIONS_H_
#define _OPTIONS_H_

typedef enum {
	ASCII,
	ISO_8859_1,
	ISO_8859_7
} charset_t;

typedef struct {
	char *font_filename;
	unsigned int font_width;
	unsigned int font_height;
	char *output_directory;

	charset_t* charsets;
	size_t charsets_size;

	char *characters;
} arguments_t;

void usage(int argc, char** argv);
void parse(int argc, char** argv, arguments_t* arguments);
void set_default_values(arguments_t* arguments);
void free_arguments(arguments_t* arguments);

#endif