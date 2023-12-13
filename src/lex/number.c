#include <ctype.h>
#include <stdint.h>

#include "lex/number.h"
#include "parse/type.h"

int scanNumber(const char *begin, const char *end, Number *num) {
  const char *p = begin;
  uint64_t x = 0;

  while (p < end && isdigit(*p)) {
    x = x * 10 + (*p - '0');
    p++;
  }

  num->x = x;
  num->ty = newType(TYPE_INT);
  return p - begin;
}
