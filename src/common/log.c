#include "common/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static const char *level_names[] = {
	"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",
};
static const char *level_color[] = {
	"\x1b[34m", "\x1b[32m", "\x1b[36m", "\x1b[33m", "\x1b[31m", "\x1b[35m",
};

void log_message(int level, const char *file, int line, const char *fmt, ...) {
	FILE *output = stdout;
	if (level >= LOG_WARN) {
		output = stderr;
	}

	struct tm *ltime = NULL;
	if (ltime == NULL) {
		time_t timer = time(NULL);
		ltime = localtime(&timer);
	}

	char buf[16] = { 0 };
	buf[strftime(buf, 16, "%H:%M:%S", ltime)] = '\0';

#ifdef _DEBUG
	fprintf(
		output, "%s %s[%s]\x1b[0m \x1b[90m%s:%d\x1b[0m - ", buf, level_color[level],
		level_names[level], file, line
	);
#else
	fprintf(output, "%s %s[%s]\x1b[0m - ", buf, level_color[level], level_names[level]);
#endif

	va_list args;
	va_start(args, fmt);
	vfprintf(output, fmt, args);
	va_end(args);

	fprintf(output, "%s", "\n");
	fflush(output);
}
