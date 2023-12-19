#ifndef LAROC_IR_IR_H
#define LAROC_IR_IR_H

#include "typedef.h"

struct Module {
  Func **funcs;
};

void printModule(Module *mod);

struct Func {
  const char *name;
  IRType *ret;
  Value **args;

  IRInst **allocas;

  Block *entry;
  Block **exits;
};

Func *newFunc(const char *name);

void printFunc(Func *func);

struct Block {
  IRInst **insts;
};

void printBlock(Block *blk);

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

void printIRInst(IRInst *inst);

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

void printValue(Value *v);

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

void printIRType(IRType *ty);

#endif
