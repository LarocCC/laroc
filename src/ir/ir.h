#ifndef LAROC_IR_IR_H
#define LAROC_IR_IR_H

#include "typedef.h"

struct Module {
  Func **funcs;
};

struct Block {
  IRInst **insts;
};

enum IRInstKind {
  IR_ALLOCA = 1, // %dst = alloca <ty>
  IR_LOAD,       // %dst = load <ptr> %src1
  IR_STORE,      // store <ptr> %src1, <ty> %src2
  IR_RETURN,     // return %src1
};

struct IRInst {
  IRInstKind kind;
  IRType *ty;

  Value *dst, *src1, *src2;
};

IRInst *newIRInst(IRInstKind kind);

enum ValKind {
  IR_VAL_INVAL,
  IR_VAL_IMM,
  IR_VAL_VAR,
};

struct Value {
  ValKind kind;
  IRType *ty;
  const char *name;
  int id;
};

Value *newValueVar(IRType *ty, const char *name);

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

IRType *newIRType(IRTypeKind kind);

#endif
