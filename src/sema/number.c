#include <stdlib.h>

#include "typedef.h"
#include "sema/number.h"
#include "sema/type.h"

Number *newIntNumber(uint64_t i, CTypeKind tyKind, CTypeAttr tyAttr) {
  Number *num = calloc(1, sizeof(Number));
  num->kind = NUM_INT;
  num->ty = newCType(tyKind, tyAttr);
  num->i = i;
  return num;
}
