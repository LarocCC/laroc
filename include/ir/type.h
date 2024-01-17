#ifndef LAROC_IR_TYPE_H
#define LAROC_IR_TYPE_H

#include "typedef.h"

typedef enum IRTypeKind {
  IR_VOID,

  IR_PTR,

  IR_I8,
  IR_I16,
  IR_I32,
  IR_I64,
  IR_U8,
  IR_U16,
  IR_U32,
  IR_U64,

  IR_F32,
  IR_F64,
} IRTypeKind;

/// An IR type.
struct IRType {
  IRTypeKind kind;
};

IRType *newIRType(IRTypeKind kind);

void printIRType(IRType *ty);

#endif
