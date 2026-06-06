#include "string.h"

void* memcpy(void* dest, const void* src, size_t n) {
    u8* d = (u8*)dest;
    const u8* s = (const u8*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const u8* a = (const u8*)s1;
    const u8* b = (const u8*)s2;
    for (size_t i = 0; i < n; i++) {
        if (a[i] != b[i]) {
            return a[i] - b[i];
        }
    }
    return 0;
}

void* memset(void* s, int c, size_t n) {
    u8* p = (u8*)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (u8)c;
    }
    return s;
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

size_t strlen(const char* s) {
    size_t len = 0;
    while (*s++) len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

void string_copy(const char* src, char* dest) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void string_append(char* dest, const char* src) {
    while (*dest) dest++;
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}
