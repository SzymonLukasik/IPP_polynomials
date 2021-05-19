/** @file
  Implementacja biblioteki służącej do bezpiecznego alokowania pamięci.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#include <stdlib.h>

#include "safealloc.h"

void* SafeCalloc(size_t n, size_t size) {
    void* p;
    p = calloc(n, size);
    if (p == NULL)
        exit(1);
    return p;
}