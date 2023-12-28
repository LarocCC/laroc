#ifndef LAROC_IR_IR_H
#define LAROC_IR_IR_H

#include <stdbool.h>
#include <stdint.h>

#include "typedef.h"

struct IRCtx {
  Func *func;
};

struct Module {
  Func **funcs;
};

void printModule(Module *mod);

struct Func {
  const char *name;
  IRType *ret;
  Value **args;

  IRInst **allocas;

  int blockCount;
  Block *entry;
  Block **exits;

  int valueCount;
  IRInst **instForValues;
};

Func *newFunc(const char *name);

void printFunc(Func *func);

struct Block {
  int id;

  IRInst **insts;
};

Block *newBlock(Func *func);

void printBlock(Block *blk);

enum IRInstKind {
  IR_NOOP,   //        noop
  IR_ALLOCA, // %dst = alloca i32 %size, i32 %align
  IR_LOAD,   // %dst = load   ptr %src1
  IR_STORE,  //        store  ptr %src1,     %src2
  IR_ADD,    // %dst = add        %src1,     %src2
  IR_SUB,    // %dst = sub        %src1,     %src2
  IR_RET,    //        ret        %src1
};

void printIRInstKind(IRInstKind kind);

struct IRInst {
  IRInstKind kind;

  Value *dst, *src1, *src2;

  bool isDAGRoot;
};

IRInst *newIRInst(IRInstKind kind);

void printIRInst(IRInst *inst);

enum ValKind {
  IR_VAL_VOID,
  IR_VAL_IMM,
  IR_VAL_VAR,
};

struct Value {
  ValKind kind;
  IRType *ty;
  int id;

  uint64_t imm;
};

Value *newValueVoid();

Value *newValueVar(Func *func, IRType *ty);

Value *newValueImm(IRType *ty, uint64_t imm);

void printValue(Value *v);

enum IRTypeKind {
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
};

struct IRType {
  IRTypeKind kind;
};

IRType *newIRType(IRTypeKind kind);

void printIRType(IRType *ty);

#endif
