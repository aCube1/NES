#include "common/utils.h"

#include "common/log.h"

#include <assert.h>
#include <stdlib.h>

void *xcalloc(unsigned long count, unsigned long size) {
	void *ptr = calloc(count, size);
	assert(ptr != NULL && "calloc() failed!");

	return ptr;
}

void *xrealloc(void *ptr, unsigned long size) {
	void *tmp = realloc(ptr, size);
	assert(tmp != NULL && "calloc() failed!");

	return tmp;
}

FILE *xfopen(const char *filename, const char *mode) {
	FILE *file = fopen(filename, mode);
	if (file == NULL) {
		log_fatal("fopen() failed: %s, %s", filename, mode);
		exit(EXIT_FAILURE);
	}

	return file;
}

FILE *xfreopen(const char *filename, const char *mode, FILE *stream) {
	FILE *file = freopen(filename, mode, stream);
	if (file == NULL) {
		log_fatal("freopen() failed: %s, %s", filename, mode);
		exit(EXIT_FAILURE);
	}

	return file;
}
