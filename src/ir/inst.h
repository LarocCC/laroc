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

#endif
