#ifndef LAROC_IR_VALUE_H
#define LAROC_IR_VALUE_H

#include "typedef.h"

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

#endif
