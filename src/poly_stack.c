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

void PolyStackPush(PolyStack *stack, Poly *poly) {
    if (stack->size == stack->capacity) {
        if (stack->capacity == 0)
            stack->capacity = 1;
        else
            stack->capacity *= 2;
        stack->data = (Poly*) SafeRealloc(stack->data,
                                            stack->capacity * (sizeof(Poly)));
    }
    stack->data[stack->size] = *poly;
    stack->size++;
}

Poly* PolyStackPop(PolyStack *stack) {
    assert(stack->size > 0);
    Poly* res = &stack->data[stack->size];
    stack->size--;
    if (stack->size <= (size_t) (stack->capacity / 4)) {
        stack->capacity = (size_t) (stack->capacity / 4);
        stack->data = (Poly*)
                SafeRealloc(stack->data,
                            stack->capacity * (sizeof(Poly)));
    }

    return res;
}

void PolyStackDestroy(PolyStack stack) {
    for (size_t i = 0; i < stack.size; i++)
        PolyDestroy(&stack.data[i]);
    free(stack.data);
}

