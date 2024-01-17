#ifndef LAROC_IR_INST_H
#define LAROC_IR_INST_H

#include <stdbool.h>

#include "typedef.h"

typedef enum IRInstKind {
  IR_INVAL,

  IR_ALLOCA, // %dst = alloca immSize, immAlign
  IR_LOAD,   // %dst = load   %ptr
  IR_STORE,  //        store  %ptr,  %var
  IR_LI,     // %dst = li     imm
  LI_CAST,   // %dst = cast   %src
  IR_ADD,    // %dst = add    %var1, %var2
  IR_SUB,    // %dst = sub    %var1, %var2
  IR_MUL,    // %dst = mul    %var1, %var2
  IR_DIV,    // %dst = div    %var1, %var2
  IR_MOD,    // %dst = mod    %var1, %var2
  IR_NOT,    // %dst = not    %var
  IR_SHL,    // %dst = shl    %var1, %var2
  IR_SHR,    // %dst = shr    %var1, %var2
  IR_AND,    // %dst = and    %var1, %var2
  IR_XOR,    // %dst = xor    %var1, %var2
  IR_OR,     // %dst = or     %var1, %var2
  IR_CMP_EQ, // %dst = cmp.eq %var1, %var2
  IR_CMP_LT, // %dst = cmp.lt %var1, %var2
  IR_CMP_LE, // %dst = cmp.le %var1, %var2
  IR_J,      //        j      .blk
  IR_BR,     //        br     %cond, .blkT, .blkF
  IR_PHI,    // %dsy = phi    .blk1, %var1, .blk2, %var2, ...
  IR_RET,    //        ret    %var
} IRInstKind;

void printIRInstKind(IRInstKind kind);

/// An IR instruction.
struct IRInst {
  /// The block that the current instruction *directly* belongs to. That is,
  /// this value should be NULL if the instruction is not the root of a DAG.
  IRBlock *block;
  /// The previous and next instruction in current block.
  IRInst *prev, *next;

  IRInstKind kind;

  /// The value that this instruction writes to. Note that IR is in SSA form, so
  /// no two instructions will write to a same value.
  Value *dst;
  /// The values that this instruction uses.
  Value **srcs;

  /// This instruction is a DAG root, i.e. this instruction can't be reordered.
  /// For example, load and store should not be reordered because they may have
  /// data dependencies.
  bool isDAGRoot;
  /// This instruction will be removed from block later.
  bool toBeRemoved;
};

IRInst *newIRInst(IRInstKind kind);

void irBlockAddInst(IRBlock *blk, IRInst *inst);
void irBlockRemoveInst(IRInst *inst);

void printIRInst(IRInst *inst, bool newLine);

#endif
