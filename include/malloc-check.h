#ifndef _MALLOC_CHECK_H
#define _MALLOC_CHECK_H

void malloc_check(void);
void start_malloc_check(void);

//for memory leak detector
#define malloc  ___malloc
#define free    ___free
#define realloc ___realloc
#define strndup ___strndup
#define strdup  ___strdup
void *___malloc(size_t);
void ___free(void*);
void *___realloc(void*,size_t);
char *___strndup(const char*,size_t);
char *___strdup(const char*);

#endif
