/** @file
  Implementacja funkcji służących do bezpiecznego alokowania pamięci.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#include <stdlib.h>

#include "safe_alloc.h"

void *SafeRealloc(void *ptr, size_t size) {
    void *p;
    p = realloc(ptr, size);
    if (p == NULL)
        exit(1);
    return p;
}

void *SafeCalloc(size_t n, size_t size) {
    void *p;
    p = calloc(n, size);
    if (p == NULL)
        exit(1);
    return p;
}