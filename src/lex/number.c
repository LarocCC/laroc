#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

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
  num->ty = newCType(TYPE_INT);
  return p - begin;
}

void printNumber(const Number *num) { printf("%lu", num->x); }
