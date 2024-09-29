#include <assert.h>
#include <bytecode.h>
#include <callstack.h>
#include <core/error.h>
#include <deserializer.h>
#include <module.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <value.h>
#include <interpreter.h>
#include <gc.h>

Value deserialize_value(FILE* file, Stack* st) {
  Value value;

  uint8_t type;
  fread(&type, sizeof(uint8_t), 1, file);

  switch (type) {
    case TYPE_INTEGER: {
      int32_t int_value;
      fread(&int_value, sizeof(int32_t), 1, file);
      value = MAKE_INTEGER(int_value);
      break;
    }
    case TYPE_FLOAT: {
      double float_value;
      fread(&float_value, sizeof(double), 1, file);
      value = MAKE_FLOAT(float_value);
      break;
    }

    case TYPE_STRING: {
      int32_t length;
      fread(&length, sizeof(int32_t), 1, file);

      char* string_value = GC_malloc(length + 1);
      fread(string_value, sizeof(char), length, file);
      string_value[length] = '\0';

      HeapValue* l = GC_malloc(sizeof(HeapValue));
      l->type = TYPE_STRING;
      l->length = length;
      l->as_string = string_value;

      value = MAKE_PTR(l);
      break;
    }

    default:
      THROW_FMT("Invalid value type, received %d", type);
  }

  return value;
}

Constants deserialize_constants(FILE* file, Stack *st) {
  Constants constants;

  int32_t constant_count;
  fread(&constant_count, sizeof(int32_t), 1, file);

  constants.constant_count = constant_count;

  constants.constants = GC_malloc(constant_count * sizeof(Value));
  for (int32_t i = 0; i < constant_count; i++) {
    constants.constants[i] = deserialize_value(file, st);
  }

  assert(constants.constants != NULL);

  return constants;
}

Libraries deserialize_libraries(FILE* file) {
  Libraries libraries;

  int32_t library_count;
  fread(&library_count, sizeof(int32_t), 1, file);

  libraries.num_libraries = library_count;
  libraries.libraries = GC_malloc(library_count * sizeof(Library));

  for (int32_t i = 0; i < library_count; i++) {
    int32_t length;
    fread(&length, sizeof(int32_t), 1, file);

    char* library_name = GC_malloc(length + 1);
    fread(library_name, sizeof(char), length, file);
    library_name[length] = '\0';

    Library lib;

    uint8_t is_std;
    fread(&is_std, sizeof(uint8_t), 1, file);

    int32_t function_count;
    fread(&function_count, sizeof(int32_t), 1, file);

    lib.num_functions = function_count;
    lib.is_standard = is_std;
    lib.name = library_name;

    libraries.libraries[i] = lib;
  }

  assert(libraries.libraries != NULL);

  return libraries;
}

Deserialized deserialize(FILE* file, Stack* st) {
  Constants constants_ = deserialize_constants(file, st);
  Libraries libraries = deserialize_libraries(file);

  int32_t instr_count;
  fread(&instr_count, sizeof(int32_t), 1, file);

  int32_t* instrs = malloc(instr_count * 4 * sizeof(int32_t));
  fread(instrs, sizeof(int32_t), instr_count * 4, file);

  Deserialized deserialized;
  deserialized.libraries = libraries;
  deserialized.instr_count = instr_count;
  deserialized.instrs = instrs;
  deserialized.constants = constants_;
  deserialized.stack = st;
  deserialized.call_stack.frame_pointer = 0;
  deserialized.natives = GC_malloc(libraries.num_libraries * sizeof(Native));
  deserialized.call_function = call_function;
  deserialized.call_threaded = call_threaded;

  return deserialized;
}
