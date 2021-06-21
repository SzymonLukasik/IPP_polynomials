/** @file
  Bilbioteka udostępniająca stos wielomianów.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/

#ifndef __POLY_STACK_H__
#define __POLY_STACK_H__

#include <stdlib.h>
#include "poly.h"

/** To jest struktura reprezentująca stos wielomianów. */
typedef struct PolyStack {
    Poly *data; ///< Tablica wielomianów znajdujących się na stosie
    size_t size; ///< Liczba obecnie znajdujących się na stosie wielomianów
    size_t capacity; ///< Liczba wielomianów dla których została zalokowana pamięć
} PolyStack;

/**
 * Tworzy pusty stos wielomianów.
 * @return pusty stos wielomianóœ
 */
PolyStack NewPolyStack();

/**
 * Wstawia wielomian na wierzchołek stosu.
 * @param[in, out]  stack : wskaźnik na stos wielomianów
 * @param[in]  poly : wielomian
 */
void PolyStackPush(PolyStack *stack, Poly poly);

/**
 * Usuwa wielomian z wierzchołka stosu oraz zwraca go.
 * @param[in, out]  stack : stos wielomianów z usuniętym wierzchołkiem
 * @return wielomian z wierzchołka stosu
 */
Poly PolyStackPop(PolyStack *stack);

/**
 * Zwraca wskaźnik na wielomian z wierzchołka stosu.
 * Zakłada, że stos zawiera co najmniej jeden wielomian.
 * @param[in] stack : stos wielomianów zawierający co najmniej
 *                    jeden wielomian
 * @return wskaźnik na wielomian z wierzchołka stosu
 */
Poly *PolyStackTop(PolyStack stack);

/**
 * Zwraca tablicę @p polys zawierającą @p k wielomianów z wierzchu stosu,
 * gdzie @p polys[k-1] to wielomian z wierzchołka,
 * @p polys[k-2] to wielomian pod wierzchołkiem, itd.
 * Zakłada, że stos zawiera co najmniej @p k wielomianów.
 * @param[in] stack : stos wielomianów zawierający co najmniej
 * @p k wielomianów.
 * @param[in] k : liczba wielomianów
 * @return tablica wielomianów
 */
Poly *PolysStackTop(PolyStack stack, size_t k);

/**
 * Zwraca tablicę @p polys zawierającą @p k wielomianów z wierzchu stosu,
 * gdzie @p polys[k-1] to wielomian z wierzchołka,
 * @p polys[k-2] to wielomian pod wierzchołkiem, itd.
 * Zdejmuje pierwsze @p k wielomianów ze stosu.
 * Zakłada, że stos zawiera co najmniej @p k wielomianów.
 * @param[in, out] stack : stos wielomianów zawierający co najmniej
 * @p k wielomianów.
 * @param[in] k : liczba wielomianów
 * @return tablica wielomianów
 */
Poly *PolysStackPop(PolyStack *stack, size_t k);

/**
 * Zwalnia pamięć zaalokowaną przez stos.
 * @param[in]  stack : stos wielomianów
 */
void PolyStackDestroy(PolyStack stack);

#endif // __POLY_STACK_H__