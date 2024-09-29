#ifndef STACK_H
#define STACK_H

#include <stdlib.h>
#include <string.h>
#include <value.h>

Stack *stack_new();
void stack_free(Stack *stack);
void stack_resize(Stack *st);

#define DOES_OVERFLOW(stack, n) stack->stack_pointer + n >= stack->capacity
#define DOES_UNDERFLOW(stack, n) stack->stack_pointer - n < BASE_POINTER
#define stack_push(stack, value) \
  if (DOES_OVERFLOW(stack, 1)) {stack_resize(stack);} \
  stack->values[stack->stack_pointer++] = value

#define stack_pop(stack) \
  stack->values[--stack->stack_pointer]

#define stack_pop_n(stack, n) \
  &stack->values[stack->stack_pointer -= n]

#define stack_push_n(stack, vs, n) \
  if (DOES_OVERFLOW(stack, n)) {stack_resize(stack);} \
  memcpy(&stack->values[stack->stack_pointer], vs, n * sizeof(Value)); \
  stack->stack_pointer += n

#endif  // STACK_H