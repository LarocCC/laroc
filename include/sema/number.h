#ifndef LAROC_SEMA_NUMBER_H
#define LAROC_SEMA_NUMBER_H

#include <stdint.h>

#include "sema/type.h"

/// A C integer constant or floating constant.
///
/// TODO: Support hexadecimal and octal integer constant.
/// TODO: Support integer suffix.
/// TODO: Support floating constant.
struct Number {
  uint64_t x;
  CType *ty;
};

#endif
