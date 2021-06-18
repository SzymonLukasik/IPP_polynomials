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


/** To jest struktura reprezentująca parę wielomianów.
 *  Używamy jej do pobierania dwóch wielomianów z wierzchu stosu. */
typedef struct PolyPair {
    Poly *first; ///< Pierwszy wielomian pary
    Poly *second; ///< Drugi wielomian pary
} PolyPair;

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
 * Zwraca parę pair wielomianów z wierzchu stosu, gdzie
 * pair.first to wielomian z wierzchołka, a pair.second
 * to wielomian pod wierzchołkiem. Zakłada, że stos
 * zawiera co najmniej dwa wielomiany.
 * @param[in] stack : stos wielomianów zawierający co najmniej
 *                    dwa wielomiany.
 * @return para wielomianów
 */
PolyPair DoublePolyStackTop(PolyStack stack);

/**
 * Zwalnia pamięć zaalokowaną przez stos.
 * @param[in]  stack : stos wielomianów
 */
void PolyStackDestroy(PolyStack stack);

#endif // __POLY_STACK_H__