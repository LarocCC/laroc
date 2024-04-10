#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>

#include "sema/number.h"
#include "sema/type.h"

int scanNumber(const char *begin, const char *end, Number *num) {
  const char *p = begin;
  uint64_t x = 0;

  num->radix = NUM_DEC;
  if (*p == '0') {
    if (p + 1 < end && (*(p + 1) == 'x' || *(p + 1) == 'X')) {
      num->radix = NUM_HEX;
      p += 2;
    } else {
      num->radix = NUM_OCT;
      p += 1;
    }
  }

  // TODO: Handle overflowed number.
  switch (num->radix) {
  case NUM_OCT:
    while (p < end && *p >= '0' && *p <= '9') {
      x = x * 8 + (*p - '0');
      p++;
    }
    break;

  case NUM_DEC:
    while (p < end && isdigit(*p)) {
      x = x * 10 + (*p - '0');
      p++;
    }
    break;

  case NUM_HEX:
    while (p < end && isxdigit(*p)) {
      if (isdigit(*p)) {
        x = x * 16 + (*p - '0');
      } else {
        x = x * 16 + 10 + (tolower(*p) - 'a');
      }
      p++;
    }
    break;

  default:
    assert(false && "Unknown radix");
  }

  num->x = x;
  num->ty = newCType(TYPE_INT, TYPE_ATTR_NONE);
  return p - begin;
}

void printNumber(const Number *num) {
  switch (num->radix) {
  case NUM_OCT:
    printf("(Oct) ");
    break;
  case NUM_DEC:
    printf("(Dec) ");
    break;
  case NUM_HEX:
    printf("(Hex) ");
    break;
  default:
    assert(false && "Unknown radix");
  }
  printf("%lu", num->x);
}
