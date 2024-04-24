#ifndef LAROC_LEX_NUM_H
#define LAROC_LEX_NUM_H

#include "typedef.h"
#include "sema/number.h"

/// Scan a number start from \p begin and end before \p end, store the result to
/// \p x and return the characters consumed.
int scanNumber(LexCtx *ctx, const char *begin, const char *end, Number *num);

void printNumber(const Number *num);

#endif
