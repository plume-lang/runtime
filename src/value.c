#include <core/error.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <value.h>
#include <gc.h>

char* constructor_name(Value v) {
  ASSERT(get_type(v) == TYPE_LIST,
         "Cannot get constructor name of non-list value");

  HeapValue* arr = GET_PTR(v);
  Value* data = arr->as_ptr;

  ASSERT(arr->length > 0, "Cannot get constructor name of empty value");
  ASSERT(get_type(data[0]) == TYPE_SPECIAL,
         "Cannot get constructor name of non-type value");
  ASSERT(get_type(data[1]) == TYPE_STRING, "Constructor name must be a string");

  return GET_STRING(data[1]);
}

Value equal(Value x, Value y) {
  ValueType x_type = get_type(x);
  ASSERT(x_type == get_type(y), "Cannot compare values of different types");

  switch (x_type) {
    case TYPE_INTEGER:
      return MAKE_INTEGER(x == y);
    case TYPE_FLOAT:
      return MAKE_INTEGER(x == y);
    case TYPE_STRING:
      return MAKE_INTEGER(strcmp(GET_STRING(x), GET_STRING(y)) == 0);
    case TYPE_LIST: {
      HeapValue* x_heap = GET_PTR(x);
      HeapValue* y_heap = GET_PTR(y);
      if (x_heap->length != y_heap->length) {
        return MAKE_INTEGER(0);
      }

      Value* x_values = x_heap->as_ptr;
      Value* y_values = y_heap->as_ptr;

      for (uint32_t i = 0; i < x_heap->length; i++) {
        if ((int32_t)!equal(x_values[i], y_values[i])) {
          return MAKE_INTEGER(0);
        }
      }

      return MAKE_INTEGER(1);
    }
    case TYPE_SPECIAL: {
      return MAKE_INTEGER(1);
    }
    default:
      THROW("Cannot compare values of unknown type");
  }
}

void native_print(Value value) {
  if (value == 0) {
    printf("null");
    return;
  }
  ValueType val_type = get_type(value);
  switch (val_type) {
    case TYPE_INTEGER:
      printf("%d", (int32_t)value);
      break;
    case TYPE_SPECIAL:
      printf("<special>");
      break;
    case TYPE_FLOAT:
      printf("%f", GET_FLOAT(value));
      break;
    case TYPE_STRING:
      printf("%s", GET_STRING(value));
      break;
    case TYPE_LIST: {
      HeapValue* list = GET_PTR(value);
      printf("[");
      if (list->length == 0) {
        printf("]");
        break;
      }
      for (uint32_t i = 0; i < list->length; i++) {
        native_print(list->as_ptr[i]);
        if (i < list->length - 1) {
          printf(", ");
        }
      }
      printf("]");
      break;
    }
    case TYPE_MUTABLE: {
      printf("<mutable ");
      native_print(GET_MUTABLE(value));
      printf(">");
      break;
    }
    case TYPE_FUNCTION: {
      printf("<function>");
      break;
    }
    case TYPE_FUNCENV: {
      printf("<funcenv>");
      break;
    }
    case TYPE_UNKNOWN:
    default: {
      printf("<unknown>");
      break;
    }
  }
}

Value MAKE_STRING(Stack* gc, char* x) {
  size_t len = strlen(x);
  HeapValue* v = allocate(gc, TYPE_STRING, len);
  v->as_string = x;
  return MAKE_PTR(v);
}

Value MAKE_LIST(Stack* gc, Value* x, uint32_t len) {
  HeapValue* v = allocate(gc, TYPE_LIST, len);
  v->as_ptr = x;
  return MAKE_PTR(v);
}

Value MAKE_MUTABLE(Stack* gc, Value x) {
  HeapValue* v = allocate(gc, TYPE_MUTABLE, 1);
  v->as_ptr = &x;
  return MAKE_PTR(v);
}

// function allocateMemory(size):
//     if available memory < size:
//         garbageCollect()

//     if available memory < size:
//         return null  // Out of memory

//     allocate memory of given size
//     return pointer to allocated memory
HeapValue* allocate(Stack* st, ValueType type, size_t size) {
  // maybe_run_gc(st);

  // if (st->root_size >= st->root_capacity) {
  //   perror("no more memory");
  // }

  HeapValue* hp = GC_malloc(sizeof(HeapValue));

  // st->roots[st->root_size++] = hp;

  hp->type = type;
  hp->length = size;
  hp->is_marked = false;

  return hp;
}