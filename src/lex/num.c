#include <ctype.h>
#include <stdint.h>

#include "lex/num.h"

int scanNum(const char *begin, const char *end, uint64_t *x) {
  const char *p = begin;
  *x = 0;

  while (p < end && isdigit(*p)) {
    *x = *x * 10 + (*p - '0');
    p++;
  }

  return p - begin;
}
