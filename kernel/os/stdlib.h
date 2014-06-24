#ifndef STRING_H
#define STRING_H

#include "stddef.h"

void* memset(void* buf, int c, size_t n);
void* memcpy(void* dest, const void* src, size_t n);
extern void* malloc(uint32_t size); //see vmm.c

#endif
