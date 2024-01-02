#ifndef LAROC_IR_IR_H
#define LAROC_IR_IR_H

#include <stdbool.h>
#include <stdint.h>

#include "typedef.h"

struct IRCtx {
  IRFunc *func;
};

struct Module {
  IRFunc **funcs;
};

void printModule(Module *mod);

struct IRFunc {
  const char *name;
  IRType *ret;
  Value **args;

  IRInst **allocas;

  int blockCount;
  IRBlock **blocks;
  IRBlock *entry;
  IRBlock **exits;

  int valueCount;
  IRInst **instForValues;
};

IRFunc *newIRFunc(const char *name);

void printIRFunc(IRFunc *func);

struct IRBlock {
  int id;
  IRBlock **precs, **succs;

  IRInst **insts;
};

IRBlock *newIRBlock(IRFunc *func);

void printIRBlock(IRBlock *blk);

enum IRInstKind {
  IR_NOOP,   //        noop
  IR_ALLOCA, // %dst = alloca i32 %size, i32 %align
  IR_LOAD,   // %dst = load   ptr %ptr
  IR_STORE,  //        store  ptr %ptr,      %val
  IR_ADD,    // %dst = add        %src1,     %src2
  IR_SUB,    // %dst = sub        %src1,     %src2
  IR_J,      //        j      blk %blk
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
  IR_VAL_BLK,
};

struct Value {
  ValKind kind;
  IRType *ty;

  uint64_t imm;
  int id;
  IRBlock *block;
};

Value *newValueVoid();

Value *newValueVar(IRFunc *func, IRType *ty);

Value *newValueImm(IRType *ty, uint64_t imm);

Value *newValueBlock(IRBlock *block);

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

  IR_BLK,
};

struct IRType {
  IRTypeKind kind;
};

IRType *newIRType(IRTypeKind kind);

void printIRType(IRType *ty);

#endif
