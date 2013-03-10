#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "options.h"
#include "config.h"

void usage(int argc, char** argv) {
	printf("Usage: %s [options] \n\n"
		"     --help                                  Display this help screen\n"
		"     --version                               Display version information"
		"-f | --font <filename>                       Font file to read from\n"
		"-h | --height <height>                       Font height to render\n"
		"-w | --width <width>                         Font width to render\n"
		"-o | --output <directory>                    Output directory\n"
		"-c | --charsets <charset1>,<charset2>,...    List of charsets to render\n"
		"   | --list-charsets                         Display a list of supported charsets\n"
		"-C | --characters <char1>,<char2>,...        List of character codes to render"
		, argv[0]);
}

void parse(int argc, char** argv, arguments_t *arguments) {
	const char short_options[] = "f:h:w:o:c:C:";

	enum {
		OPTION_HELP = 1, OPTION_VERSION, OPTION_LISTCHARSETS
	};

	const struct option long_options[] = {
		{ "help", no_argument, NULL, OPTION_HELP},
		{ "version", no_argument, NULL, OPTION_VERSION},
		{ "font", required_argument, NULL, 'f'},
		{ "width", required_argument, NULL, 'w'},
		{ "height", required_argument, NULL, 'h'},
		{ "output", required_argument, NULL, 'o'},
		{ "charsets", required_argument, NULL, 'c'},
		{ "list-charsets", no_argument, NULL, OPTION_LISTCHARSETS},
		{ "characters", required_argument, NULL, 'C'},
		{ 0, 0, 0, 0 }
	};

	char* const charsets_opts[] = {
		[ASCII] = "ascii",
		[ISO_8859_1] = "iso-8859-1",
		[ISO_8859_7] = "iso-8859-7",
		NULL
	};

	for (;;) {
		int c = 0;

		c = getopt_long(argc, argv, short_options, long_options, NULL);

		if (c == EOF)
			break;

		switch (c) {
			case 0: // getopt_long() flag
				break;

			case 'h': // font height
				arguments->font_height = strtoul(optarg, NULL, 0);
				break;

			case 'w': // font-width
				arguments->font_width = strtoul(optarg, NULL, 0);
				break;

			case 'f':
				arguments->font_filename = optarg;
				break;

			case 'o':
				arguments->output_directory = optarg;
				break;

			case 'c':
				{
					char *value;
					char *subopts = optarg;

					while (*subopts != '\0') {
						int option = getsubopt(&subopts, charsets_opts, &value);

						if (option == -1) {
							printf("Unknown charset: '%s'\n", value);
							exit(EXIT_FAILURE);
						}

						arguments->charsets_size++;
						arguments->charsets = realloc(arguments->charsets, sizeof(charset_t)*arguments->charsets_size);
						arguments->charsets[arguments->charsets_size-1] = option;

					}
				}
				break;

			case 'C':
				arguments->characters = optarg;
				break;

			case OPTION_HELP: // help
				usage(argc, argv);
				exit(EXIT_SUCCESS);
				break;

			case OPTION_VERSION: // version
				printf(PACKAGE_STRING ", " PACKAGE_URL "\n");
				exit(EXIT_SUCCESS);
				break;

			case OPTION_LISTCHARSETS: // list charsets
				{
					size_t i = 0;
					while (charsets_opts[i] != NULL) {
						printf("%s\n", charsets_opts[i]);
						i++;
					}
					exit(EXIT_SUCCESS);
				}
				break;

			default:
			case ':': // missing option argument
			case '?': // unknown or ambiguous option
				usage(argc, argv);
				exit(EXIT_FAILURE);
				break;
		}
	}

	// parameter file MUST be specified
	if (arguments->font_filename == NULL) {
		fprintf(stderr, "You must specify a font using the --font option!\n\n");
		usage(argc, argv);
		exit(EXIT_FAILURE);
	}

	if (arguments->output_directory == NULL) {
		fprintf(stderr, "You must specify an output directory using the --output option!\n\n");
		usage(argc, argv);
		exit(EXIT_FAILURE);
	}

	if ((arguments->font_width == 0) && (arguments->font_height == 0)) {
		fprintf(stderr, "You must set the font height or width using the --height or --width option!\n\n");
		usage(argc, argv);
		exit(EXIT_FAILURE);
	}
}

void free_arguments(arguments_t* arguments) {
	free(arguments->charsets);
}

void set_default_values(arguments_t* arguments) {
	arguments->output_directory = NULL;
	arguments->font_filename = NULL;
	arguments->font_width = 0;
	arguments->font_height = 0;

	arguments->charsets_size = 0;
	arguments->charsets = malloc(sizeof(charset_t)*arguments->charsets_size);

	arguments->characters = NULL;
}
