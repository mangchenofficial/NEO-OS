#ifndef STRING_H
#define STRING_H

#include "types.h"

void* memcpy(void* dest, const void* src, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
void* memset(void* s, int c, size_t n);
char* strcpy(char* dest, const char* src);
size_t strlen(const char* s);
int strcmp(const char* s1, const char* s2);
void string_copy(const char* src, char* dest);
void string_append(char* dest, const char* src);

#endif
