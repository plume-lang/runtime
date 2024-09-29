#include <core/error.h>
#include <stack.h>
#include <stdio.h>
#include <stdlib.h>
#include <value.h>
#include <gc.h>

Stack* stack_new() {
  Stack* stack = malloc(sizeof(Stack));
  stack->stack_pointer = BASE_POINTER;
  stack->capacity = MAX_STACK_SIZE;
  stack->values = malloc(sizeof(Value) * stack->capacity);
  return stack;
}

void stack_free(Stack* stack) {
  free(stack->values);
  free(stack);
}

void stack_resize(Stack *st) {
  st->capacity *= 2;
  Value* new_st = realloc(st->values, st->capacity * sizeof(Value));

  if (new_st == NULL) {
    // Handle allocation failure
    fprintf(stderr, "Failed to grow stack\n");
    return;
  }

  st->values = new_st;
}
