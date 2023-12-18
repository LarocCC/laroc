#ifndef LAROC_IR_IR_H
#define LAROC_IR_IR_H

#include "typedef.h"

struct Module {
  Func **funcs;
};

struct Func {
  const char *name;
  IRType *ret;
  Val **params;
};

Func *newFunc(const char *name);

enum ValKind {
  IR_VAL_INVAL,
  IR_VAL_IMM,
  IR_VAL_VAR,
};

struct Val {
  ValKind kind;
  IRType *ty;
  const char *name;
  int id;
};

Val *newValVar(IRType *ty, const char *name);

enum IRTypeKind {
  IR_PTR = 1,
  IR_I8,
  IR_I16,
  IR_I32,
  IR_I64,
  IR_U8,
  IR_U16,
  IR_U32,
  IR_U64,
};

struct IRType {
  IRTypeKind kind;
};

IRType *newIRTypeFromCType(CType *cty);

#endif
