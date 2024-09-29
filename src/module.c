#include <module.h>

Frame pop_frame(Deserialized *mod) {
  Value clos_env = mod->stack->values[mod->base_pointer];

  ASSERT_FMT(get_type(clos_env) == TYPE_FUNCENV, "Expected closure environment got %s", type_of(clos_env));

  reg pc = (int16_t) GET_NTH_ELEMENT(clos_env, 0);
  size_t old_sp = (int16_t) GET_NTH_ELEMENT(clos_env, 1);
  size_t base_ptr = (int16_t) GET_NTH_ELEMENT(clos_env, 2);

  mod->call_stack.frame_pointer--;

  return (Frame) { pc, old_sp, base_ptr };
}