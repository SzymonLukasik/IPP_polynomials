#ifndef __SAFEALLOC_H__
#define __SAFEALLOC_H__

#include <stdlib.h>

void *SafeMalloc(size_t size);

void *SafeRealloc(void *ptr, size_t size);

void *SafeCalloc(size_t n, size_t size);

#endif /* __SAFEALLOC_H__ */