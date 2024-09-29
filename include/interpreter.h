#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <module.h>

Value call_function(struct Deserialized *mod, Value callee, int32_t argc, Value* argv);
Value call_threaded(struct Deserialized *mod, Value callee, int32_t argc, Value* argv);
Value run_interpreter(struct Deserialized *deserialized, int32_t ipc, bool does_return, int32_t current_callstack);

#endif  // INTERPRETER_H