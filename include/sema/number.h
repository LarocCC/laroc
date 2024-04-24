#ifndef LAROC_SEMA_NUMBER_H
#define LAROC_SEMA_NUMBER_H

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

#endif
