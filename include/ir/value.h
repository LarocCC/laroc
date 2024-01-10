#ifndef LAROC_IR_VALUE_H
#define LAROC_IR_VALUE_H

#include "typedef.h"

typedef enum ValKind {
  IR_VAL_VOID,     // Void value
  IR_VAL_IMM,      // Immediate
  IR_VAL_VAR,      // Variable
  IR_VAL_BLOCK,    // Block
  IR_VAL_DAG_NODE, // DAG node
} ValKind;

/// An IR value.
///
/// TODO: Use unions to save memory.
struct Value {
  ValKind kind;

  /// For IR_VAL_VAR and IR_VAL_DAG_NODE. ID of the variable.
  int id;
  /// For IR_VAL_VAR. The IR type of the variable
  IRType *ty;

  /// For IR_VAL_IMM.
  int imm;

  /// For IR_VAL_BLOCK.
  IRBlock *block;

  /// For IR_VAL_DAG_NODE. The instruction generating the current IR value.
  /// Value::inst->dst->id should be identical to Value::id.
  IRInst *inst;
};

Value *newValueVoid();
Value *newValueVar(IRFunc *func, IRType *ty);
Value *newValueImm(int imm);
Value *newValueBlock(IRBlock *block);

Value *newValueDAGNode(IRInst *inst);

void printValue(Value *v);

#endif
