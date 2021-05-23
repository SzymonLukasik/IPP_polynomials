#ifndef __POLY_STACK_H__
#define __POLY_STACK_H__

#include <stdlib.h>
#include "poly.h"

typedef struct PolyStack {
    Poly* data; // Tablica wielomianów znajdujących się na stosie
    size_t size; // Liczba obecnie znajdujących się na stosie wielomianów
    size_t capacity; // Liczba wielomianów dla których została zalokowana pamięć
} PolyStack;

typedef struct PolyPair {
    Poly *first;
    Poly *second;
} PolyPair;

/**
 * Tworzy pusty stos wielomianów.
 * @return pusty stos wielomianóœ
 */
PolyStack NewPolyStack();

/**
 * Wstawia wielomian na wierzchołek stosu.
 * @param[in]  stack : stos wielomianów
 * @param[in]  poly : wielomian
 */
void PolyStackPush(PolyStack* stack, Poly poly);

/**
 * Usuwa wielomian z wierzchołka stosu oraz zwraca do niego wskaźnik.
 * @param[in]  stack : stos wielomianów z usuniętym wierzchołkiem
 * @return wielomian z wierzchołka stosu
 */
Poly PolyStackPop(PolyStack* stack);

Poly* PolyStackTop(PolyStack stack);

PolyPair DoublePolyStackTop(PolyStack stack);

/**
 * Zwalnia pamięć zaalokowaną przez stos.
 * @param[in]  stack : stos wielomianów
 */
void PolyStackDestroy(PolyStack stack);

#endif // __POLY_STACK_H__