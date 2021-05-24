/** @file
  Bilbioteka udostępniająca stos jednomianów.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#ifndef __MONO_STACK_H__
#define __MONO_STACK_H__

#include <stdlib.h>
#include "poly.h"

/** To jest struktura reprezentująca stos jednomianów. */
typedef struct MonoStack {
    Mono *data; ///< Tablica jednomianów znajdujących się na stosie
    size_t size; ///< Liczba obecnie znajdujących się na stosie jednomianów
    size_t capacity; ///< Liczba jednomianów dla których została zalokowana pamięć
} MonoStack;

/**
 * Tworzy pusty stos jednomianów.
 * @return pusty stos jednomianów
 */
MonoStack NewMonoStack();

/**
 * Wstawia jednomian na wierzchołek stosu.
 * @param[out]  stack : wskaźnik na stos jednomianów
 * @param[in]  poly : jednomian
 */
void MonoStackPush(MonoStack *stack, Mono mono);

/**
 * Zwalnia pamięć zaalokowaną przez stos.
 * @param[in]  stack : stos jednomianów
 */
void MonoStackDestroy(MonoStack stack);

#endif // __MONO_STACK_H__