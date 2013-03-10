#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "output.h"

int	output_check_directory(const char* output_directory) {
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

	return 0;
}