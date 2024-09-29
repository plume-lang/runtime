#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
  OP_LoadLocal,
  OP_StoreLocal,
  OP_LoadConstant,
  OP_LoadGlobal,
  OP_StoreGlobal,
  OP_Return,
  OP_Compare,
  OP_And,
  OP_Or,
  OP_LoadNative,
  OP_MakeList,
  OP_ListGet,
  OP_Call,
  OP_JumpElseRel,
  OP_TypeOf,
  OP_ConstructorName,
  OP_Phi,
  OP_MakeLambda,
  OP_GetIndex,
  OP_Special,
  OP_JumpRel,
  OP_Slice,
  OP_ListLength,
  OP_Halt,
  OP_Update,
  OP_MakeMutable,
  OP_UnMut,
  OP_Add,
  OP_Sub,
  OP_ReturnConst,
  OP_AddConst,
  OP_SubConst,
  OP_JumpElseRelCmp,
  OP_IJumpElseRelCmp,
  OP_JumpElseRelCmpConst,
  OP_IJumpElseRelCmpConst,
  OP_CallGlobal,
  OP_CallNative,
  OP_MakeAndStoreLambda,
} Opcode;

typedef struct {
  Opcode opcode;
  int32_t operand1;
  int32_t operand2;
  int32_t operand3;
} Instruction;

typedef struct {
  Instruction *instructions;
  int32_t instruction_count;
} Bytecode;

typedef struct {
  char *name;
  int32_t num_functions;
  int is_standard;
} Library;

typedef struct {
  Library *libraries;
  int32_t num_libraries;
} Libraries;

typedef enum {
  LessThan = 0,
  GreaterThan = 1,
  EqualTo = 2,
  NotEqualTo = 3,
  LessThanOrEqualTo = 4,
  GreaterThanOrEqualTo = 5,
  And = 6,
  Or = 7,
} Comparison;

#endif  // BYTECODE_H
