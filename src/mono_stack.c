#include <stdlib.h>

#include "safe_alloc.h"
#include "mono_stack.h"

MonoStack NewMonoStack() {
    MonoStack stack;
    stack.data = NULL;
    stack.size = 0;
    stack.capacity = 0;
    return stack;
}

void MonoStackPush(MonoStack *stack, Mono mono) {
    if (stack->size == stack->capacity) {
        if (stack->capacity == 0)
            stack->capacity = 1;
        else
            stack->capacity *= 2;
        stack->data = (Mono*) SafeRealloc(stack->data,
                                          stack->capacity * (sizeof(Mono)));
    }
    stack->data[stack->size] = mono;
    stack->size++;
}

void MonoStackDestroy(MonoStack stack) {
    for (size_t i = 0; i < stack.size; i++)
        MonoDestroy(&stack.data[i]);
    free(stack.data);
}

