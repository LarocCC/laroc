#ifndef LAROC_SEMA_NUMBER_H
#define LAROC_SEMA_NUMBER_H

#include <bits/stdint-uintn.h>
#include <stdint.h>

#include "sema/type.h"

typedef enum NumberKind {
  NUM_INT,
  NUM_FLOAT,
} NumberKind;

/// A C integer constant or floating constant.
struct Number {
  NumberKind kind;
  CType *ty;

  union {
    uint64_t i;
    long double f;
  };
};

Number *newIntNumber(uint64_t i, CTypeKind tyKind, CTypeAttr tyAttr);

#endif
