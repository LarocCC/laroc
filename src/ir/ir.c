#include <assert.h>
#include <stdlib.h>

#include "typedef.h"
#include "ir/ir.h"

IRInst *newIRInst(IRInstKind kind) {
  IRInst *inst = calloc(1, sizeof(IRInst));
  inst->kind = kind;
  return inst;
}

Val *newValVar(IRType *ty, const char *name) {
  static int id = 0;

  Val *x = calloc(1, sizeof(Val));
  x->kind = IR_VAL_VAR;
  x->ty = ty;
  x->name = name;
  x->id = ++id;
  return x;
}

IRType *newIRType(IRTypeKind kind) {
  IRType *ty = calloc(1, sizeof(IRType));
  ty->kind = kind;
  return ty;
}
