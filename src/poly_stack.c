/** @file
  Implementacja biblioteki udostępniającej stos wielomianów.

  @author Szymon Łukasik <sl428760@mimuw.students.edu.pl>
  @date 2021
*/
#include <stdlib.h>

#include "safe_alloc.h"
#include "poly_stack.h"

PolyStack NewPolyStack() {
    PolyStack stack;
    stack.data = NULL;
    stack.size = 0;
    stack.capacity = 0;
    return stack;
}

void PolyStackPush(PolyStack *stack, Poly poly) {
    if (stack->size == stack->capacity) {
        if (stack->capacity == 0)
            stack->capacity = 1;
        else
            stack->capacity *= 2;
        stack->data = (Poly *) SafeRealloc(stack->data,
                                           stack->capacity * (sizeof(Poly)));
    }
    stack->data[stack->size] = poly;
    stack->size++;
}

Poly PolyStackPop(PolyStack *stack) {
    assert(stack->size > 0);
    Poly res = stack->data[stack->size - 1];
    stack->size--;
    if (stack->size > 0 && stack->size <= (size_t) (stack->capacity / 4)) {
        stack->capacity = stack->size;
        stack->data = (Poly *)
                SafeRealloc(stack->data,
                            stack->size * (sizeof(Poly)));
    }
    return res;
}

Poly *PolyStackTop(PolyStack stack) {
    assert(stack.size >= 1);
    return &stack.data[stack.size - 1];
}

Poly *PolysStackTop(PolyStack stack, size_t k) {
    assert(stack.size >= k);
    Poly *polys = SafeCalloc(k, sizeof(Poly));
    for (size_t i = 1; i <= k; i++)
        polys[k - i] = stack.data[stack.size - i];
    return polys;
}

Poly *PolysStackPop(PolyStack *stack, size_t k) {
    Poly *polys = PolysStackTop(*stack, k);
    while (k--)
        PolyStackPop(stack);
    return polys;
}

void PolyStackDestroy(PolyStack stack) {
    for (size_t i = 0; i < stack.size; i++)
        PolyDestroy(&stack.data[i]);
    free(stack.data);
}

