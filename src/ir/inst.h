#ifndef LAROC_IR_INST_H
#define LAROC_IR_INST_H

#include <stdbool.h>

#include "typedef.h"

typedef enum IRInstKind {
  IR_INVAL,

  IR_ALLOCA, // %dst = alloca i32 %size, i32 %align
  IR_LOAD,   // %dst = load   %ptr
  IR_STORE,  //        store  %ptr,  %var
  IR_ADD,    // %dst = add    %var1, %var2
  IR_SUB,    // %dst = sub    %var1, %var2
  IR_LI,     // %dst = li     %imm
  IR_J,      //        j      %blk
  IR_RET,    //        ret    %var
} IRInstKind;

void printIRInstKind(IRInstKind kind);

struct IRInst {
  IRBlock *block;
  IRInst *prev, *next;

  IRInstKind kind;

  Value *dst;
  Value **srcs;

  bool isDAGRoot;
  bool toBeRemoved;
};

IRInst *newIRInst(IRInstKind kind);

void irBlockAddInst(IRBlock *blk, IRInst *inst);
void irBlockRemoveInst(IRInst *inst);

void printIRInst(IRInst *inst, bool newLine);

typedef enum ValKind {
  IR_VAL_VOID,
  IR_VAL_IMM,
  IR_VAL_VAR,
  IR_VAL_BLOCK,
  IR_VAL_DAG_NODE,
} ValKind;

struct Value {
  ValKind kind;
  IRType *ty;

  int imm;
  int id;
  IRBlock *block;

  IRInst *inst;
};

Value *newValueVoid();
Value *newValueVar(IRFunc *func, IRType *ty);
Value *newValueImm(IRType *ty, int imm);
Value *newValueBlock(IRBlock *block);

Value *newValueDAGNode(IRInst *inst);

void printValue(Value *v);

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

  IR_BLOCK,
} IRTypeKind;

struct IRType {
  IRTypeKind kind;
};

IRType *newIRType(IRTypeKind kind);

void printIRType(IRType *ty);

#endif
