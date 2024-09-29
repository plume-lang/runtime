#ifndef VALUE_H
#define VALUE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef uint64_t Value;

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
typedef HANDLE thread_t;
#else
#include <pthread.h>
typedef pthread_t thread_t;
#endif

// Masks for important segments of a float value
#define MASK_SIGN 0x8000000000000000
#define MASK_EXPONENT 0x7ff0000000000000
#define MASK_QUIET 0x0008000000000000
#define MASK_TYPE 0x0007000000000000
#define MASK_SIGNATURE 0xffff000000000000
#define MASK_PAYLOAD_PTR 0x0000ffffffffffff
#define MASK_PAYLOAD_INT 0x00000000ffffffff

// Type IDs for short encoded types
#define MASK_TYPE_NAN 0x0000000000000000
#define MASK_TYPE_SPECIAL 0x0001000000000000
#define MASK_TYPE_INTEGER 0x0002000000000000
#define MASK_TYPE_STRING 0x0003000000000000
#define MASK_TYPE_FUNCTION 0x0004000000000000
#define MASK_TYPE_FUNCENV 0x0005000000000000

// Constant short encoded values
#define kNaN (MASK_EXPONENT | MASK_QUIET)
#define kNull (kNaN | MASK_TYPE_SPECIAL)

// Signatures of encoded types
#define SIGNATURE_NAN kNaN
#define SIGNATURE_SPECIAL kNull
#define SIGNATURE_INTEGER (kNaN | MASK_TYPE_INTEGER)
#define SIGNATURE_STRING (kNaN | MASK_TYPE_STRING)
#define SIGNATURE_FUNCTION (kNaN | MASK_TYPE_FUNCTION)
#define SIGNATURE_FUNCENV (kNaN | MASK_TYPE_FUNCENV)
#define SIGNATURE_POINTER (kNaN | MASK_SIGN)

#define MAX_ROOT_SET 1000  // Adjust as needed

typedef int32_t reg;

Value equal(Value x, Value y);
char* constructor_name(Value x);
void native_print(Value value);

// The type of the stored value
typedef enum {
  TYPE_INTEGER = 0,
  TYPE_FLOAT,
  TYPE_STRING,
  TYPE_LIST,
  TYPE_SPECIAL,
  TYPE_MUTABLE,
  TYPE_FUNCTION,
  TYPE_FUNCENV,
  TYPE_UNKNOWN,
  TYPE_API,
  TYPE_THREAD,
} ValueType;

// Container for arrays
typedef struct {
  Value* data;
  uint32_t length;
} Array;

typedef struct {
  char* data;
  uint32_t length;
} String;

// Container type for values
typedef struct {
  ValueType type;
  uint32_t length;
  bool is_marked;

  union {
    char* as_string;
    Value* as_ptr;
    void* as_any;
    thread_t as_thread;
  };
} HeapValue;

#define GLOBALS_SIZE 1024
#define MAX_STACK_SIZE GLOBALS_SIZE * 32
#define VALUE_STACK_SIZE MAX_STACK_SIZE - GLOBALS_SIZE
#define BASE_POINTER GLOBALS_SIZE
#define INITIAL_HEAP_CAPACITY 1024
#define GROWTH_FACTOR 2

typedef struct {
  Value *values;
  int16_t stack_pointer;
  int32_t capacity;
} Stack;

#define MAKE_INTEGER(x) (SIGNATURE_INTEGER | (uint32_t)(x))
#define MAKE_FLOAT(x) (*(Value*)(&(x)))
#define MAKE_PTR(x) (SIGNATURE_POINTER | (uint64_t)(x))

#define MAKE_FUNCTION(x, y) \
  (SIGNATURE_FUNCTION | (uint16_t)(x) | ((uint16_t)(y) << 16))
#define MAKE_FUNCENV(pc, sp, bp)                                 \
  (SIGNATURE_FUNCENV | (uint64_t)(pc) | ((uint64_t)(sp) << 16) | \
   ((uint64_t)(bp) << 32))

Value MAKE_MUTABLE(Stack* gc, Value x);
Value MAKE_STRING(Stack* gc, char* x);
Value MAKE_LIST(Stack* gc, Value* x, uint32_t length);

#define MAKE_SPECIAL() kNull
#define MAKE_ADDRESS(x) MAKE_INTEGER(x)
#define MAKE_NATIVE(x) MAKE_STRING(x, strlen(x))

#define GET_PTR(x) ((HeapValue*)((x) & MASK_PAYLOAD_PTR))
#define GET_STRING(x) GET_PTR(x)->as_string
#define GET_LIST(x) GET_PTR(x)->as_ptr
#define GET_MUTABLE(x) *(GET_PTR(x)->as_ptr)

#define GET_INT(x) ((x) & MASK_PAYLOAD_INT)
#define GET_FLOAT(x) (*(double*)(&(x)))
#define GET_ADDRESS(x) GET_INT(x)
#define GET_NATIVE(x) GET_STRING(x)
#define GET_NTH_ELEMENT(x, n) ((x >> (n * 16)) & MASK_PAYLOAD_INT)

#define IS_PTR(x) (((x) & MASK_SIGNATURE) == SIGNATURE_POINTER)
#define IS_FUN(x) (((x) & MASK_SIGNATURE) == SIGNATURE_FUNCTION)

static inline ValueType get_type(Value value) {
  uint64_t signature = value & MASK_SIGNATURE;
  if ((~value & MASK_EXPONENT) != 0) return TYPE_FLOAT;

  // Check for encoded pointer
  if (signature == SIGNATURE_POINTER) {
    HeapValue* ptr = GET_PTR(value);
    return ptr->type;
  }

  // Short encoded types
  switch (signature) {
    case SIGNATURE_NAN:
      return TYPE_UNKNOWN;
    case SIGNATURE_SPECIAL:
      return TYPE_SPECIAL;
    case SIGNATURE_INTEGER:
      return TYPE_INTEGER;
    case SIGNATURE_FUNCTION:
      return TYPE_FUNCTION;
    case SIGNATURE_FUNCENV:
      return TYPE_FUNCENV;
  }

  return TYPE_UNKNOWN;
}

static inline char* type_of(Value value) {
  switch (get_type(value)) {
    case TYPE_INTEGER:
      return "integer";
    case TYPE_FUNCTION:
      return "function";
    case TYPE_FUNCENV:
      return "function_env";
    case TYPE_FLOAT:
      return "float";
    case TYPE_STRING:
      return "string";
    case TYPE_LIST:
      return "list";
    case TYPE_SPECIAL:
      return "special";
    case TYPE_MUTABLE:
      return "mutable";
    case TYPE_UNKNOWN:
      return "unknown";
    case TYPE_API:
      return "api";
    case TYPE_THREAD:
      return "thread";
  }
}

HeapValue* allocate(Stack* st, ValueType type, size_t size);

#endif  // VALUE_H
