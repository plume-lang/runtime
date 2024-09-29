#ifndef CALLSTACK_H
#define CALLSTACK_H

#include <stdlib.h>
#include <value.h>
#include <core/error.h>
#include <core/debug.h>
#include <stdio.h>

#define MAX_FRAMES 1024

typedef struct {
  reg instruction_pointer;
  int32_t stack_pointer;
  int32_t base_ptr;
} Frame;

typedef struct {
  int32_t frame_pointer;

  Frame frames[MAX_FRAMES];
} CallStack;

CallStack *callstack_new();
void callstack_free(CallStack *callstack);

#endif  // CALLSTACK_H