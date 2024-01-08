#ifndef LAROC_LEX_NUM_H
#define LAROC_LEX_NUM_H

#include <stdint.h>

#include "typedef.h"
#include "parse/type.h"

struct Number {
  uint64_t x;
  CType *ty;
};

/// Scan a number start from \p begin and end before \p end, store the result to
/// \p x and return the characters consumed.
int scanNumber(const char *begin, const char *end, Number *num);

void printNumber(const Number *num);

#endif
