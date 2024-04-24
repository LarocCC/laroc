#ifndef LAROC_SEMA_NUMBER_H
#define LAROC_SEMA_NUMBER_H

#include <stdint.h>

#include "sema/type.h"

typedef enum NumberRadix {
  // C99 (6.4.4.1) octal-constant
  NUM_OCT = 8,
  // C99 (6.4.4.1) decimal-constant
  // C99 (6.4.4.2) decimal-floating-constant
  NUM_DEC = 10,
  // C99 (6.4.4.1) hexadecimal-constant
  // C99 (6.4.4.2) hexadecimal-floating-constant
  NUM_HEX = 16,
} NumberRadix;

typedef enum NumberSuffix {
  NUM_SUFFIX_NONE = 0,
  NUM_U = 1 << 1,
  NUM_L = 1 << 2,
  NUM_LL = 1 << 3,
} NumberSuffix;

/// A C integer constant or floating constant.
///
/// TODO: Support floating constant.
struct Number {
  uint64_t x;

  NumberRadix radix;
  NumberSuffix suffix;
  CType *ty;
};

#endif
