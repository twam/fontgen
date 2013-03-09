#include <iconv.h>
#include <stdio.h>
#include <errno.h>

#include "convert.h"

size_t convert_string(const char *charset_output, const char *charset_input, char* input, size_t input_size, char* output, size_t output_size) {
    // out, in
    iconv_t converter = iconv_open(charset_output, charset_input);
    if (converter == (iconv_t) -1) {
      perror ("iconv_open");
      return -1;
    }

    size_t output_available_size = output_size;
    size_t result_code = iconv(converter, &input, &input_size, &output, &output_available_size);
    if (result_code == -1)
    {
    	switch (errno) {
    		case EILSEQ:
    			fprintf(stderr, "Illegal byte sequence 0x%hhX when converting from %s to %s.\n", input[0], charset_input, charset_output);
    			break;

    		default:
   		        perror("iconv");
   		        break;
    	}
        return -1;
    }

    // cleanup
    iconv(converter, NULL, NULL, &output, &output_available_size);

	if (iconv_close(converter) != 0) {
		perror("iconv_close");
		return -1;
	}

    return output_size-output_available_size;
}