#ifndef MODULE_H
#define MODULE_H

#include <bytecode.h>
#include <core/library.h>
#include <stack.h>
#include <stdlib.h>
#include <value.h>
#include <callstack.h>
// #include <gc.h>

// #define malloc(size) GC_malloc(size)
// #define calloc(n, size) GC_malloc(n * size)
// #define realloc(ptr, size) GC_realloc(ptr, size)

typedef struct {
  Value* constants;
  int32_t constant_count;
} Constants;

typedef struct Deserialized {
  Libraries libraries;
  
  int32_t instr_count;
  int32_t *instrs;

  int32_t base_pointer;
  CallStack call_stack;

  Constants constants;
  Stack *stack;
  struct {
    Value (**functions)(int argc, struct Deserialized *des, Value *args);
  } *natives;
  DLL* handles;

  int32_t argc;
  Value *argv;

  int32_t pc;
  Value (*call_function)(struct Deserialized *m, Value callee, int32_t argc, Value* argv);
  Value (*call_threaded)(struct Deserialized *m, Value callee, int32_t argc, Value* argv);
} Deserialized;

typedef Value (*Native)(int argc, struct Deserialized *m, Value *args);

typedef Deserialized Module;

Frame pop_frame(Deserialized *mod);

#endif  // MODULE_H