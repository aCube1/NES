#ifndef _COMMON_UTILS_H_
#define _COMMON_UTILS_H_

#include <stdio.h>

#define flag_set(data, flag)   ((data) |= (flag))
#define flag_clear(data, flag) ((data) &= ~(flag))
#define flag_check(data, flag) ((data) & (flag))

void *xcalloc(unsigned long count, unsigned long size);
void *xrealloc(void *ptr, unsigned long size);

FILE *xfopen(const char *filename, const char *mode);
FILE *xfreopen(const char *filename, const char *mode, FILE *stream);

#endif /* _COMMON_H_ */
