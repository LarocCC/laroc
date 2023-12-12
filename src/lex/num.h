#ifndef LAROC_LEX_NUM_H
#define LAROC_LEX_NUM_H

#include <stdint.h>

/// Scan a number start from \p begin and end before \p end, store the result to
/// \p x and return the characters consumed.
int scanNum(const char *begin, const char *end, uint64_t *x);

#endif
