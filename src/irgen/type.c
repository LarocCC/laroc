#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ir/type.h"
#include "sema/type.h"

IRType *newIRTypeFromCType(CType *cTy) {
  IRType *ty = calloc(1, sizeof(IRType));

  switch (cTy->kind) {
  case TYPE_BOOL:
  case TYPE_CHAR:
    ty->kind = cTy->attr & TYPE_ATTR_UNSIGNED ? IR_U8 : IR_I8;
    break;

  case TYPE_SHORT:
    ty->kind = cTy->attr & TYPE_ATTR_UNSIGNED ? IR_U16 : IR_I16;
    break;

  case TYPE_INT:
    ty->kind = cTy->attr & TYPE_ATTR_UNSIGNED ? IR_U32 : IR_I32;
    break;

  case TYPE_LONG:
  case TYPE_LONG_LONG:
    ty->kind = cTy->attr & TYPE_ATTR_UNSIGNED ? IR_U64 : IR_I64;
    break;

  case TYPE_VOID:
    ty->kind = IR_VOID;
    break;

  default:
    assert(false);
  }

  return ty;
}
