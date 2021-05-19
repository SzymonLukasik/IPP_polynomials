/** @file
  Biblioteka służąca do bezpiecznego alokowania pamięci.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#ifndef __SAFEALLOC_H__
#define __SAFEALLOC_H__

#include <stdlib.h>

/**
 * Alokuje pamięć przy użyciu funkcji malloc,
 * a w przypadku niepowodzenia kończy działanie programu z kodem 1.
 * @param[in] size : ilość pamięci do zaalokowania (w bajtach)
 * @return wskaźnik na zaalkowaną pamięć
 */
void* SafeMalloc(size_t size);

/**
 * Realokuje pamięć przy użyciu funkcji realloc,
 * a w przypadku niepowodzenia kończy działanie programu z kodem 1.
 * @param[in] ptrt : wskaźnik na pamięć do realokacji
 * @param[in] size : ilość pamięci do zaalokowania (w bajtach)
 * @return wskaźnik na początek zaalokowanej pamięci
 */
void* SafeRealloc(void* ptr, size_t size);

/**
 * Alokuje pamięć blokową przy użyciu funkcji calloc,
 * a w przypadku niepowodzenia kończy działanie programu z kodem 1.
 * @param[in] n : ilość bloków do zaalokowania
 * @param[in] size : pamięć potrzebna do zaalokowania jednego bloku (w bajtach)
 * @return wskaźnik na początek zaalokowanej pamięci
 */
void* SafeCalloc(size_t n, size_t size);

#endif // __SAFEALLOC_H__