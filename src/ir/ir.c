#include <assert.h>
#include <stdlib.h>

#include "ir/ir.h"
#include "parse/type.h"

IRType *newIRTypeFromCType(CType *cTy) {
  IRType *irTy = calloc(1, sizeof(IRType));
  switch (cTy->kind) {
  case TYPE_INT:
    irTy->kind = IR_I32;
    break;
  default:
    assert(false);
  }
  return irTy;
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

Func *newFunc(const char *name) {
  Func *func = calloc(1, sizeof(Func));
  func->name = name;
  return func;
}
