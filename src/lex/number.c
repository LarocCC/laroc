#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>

#include "typedef.h"
#include "lex/lex.h"
#include "sema/number.h"
#include "sema/type.h"
#include "util/diag.h"

static CType *inferIntegerType(Number *num);

int scanNumber(LexCtx *ctx, const char *begin, const char *end, Number *num) {
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

  while (isalpha(*p)) {
    switch (*p) {
    case 'u':
    case 'U':
      switch (num->suffix) {
      case NUM_SUFFIX_NONE:
      case NUM_L:
      case NUM_LL:
        num->suffix |= NUM_U;
        p++;
        continue;
      default:
        goto reject_integer_suffix;
      }

    case 'l':
    case 'L':
      // ll, LL
      if (*(p + 1) == *p) {
        switch (num->suffix) {
        case NUM_SUFFIX_NONE:
        case NUM_U:
          num->suffix |= NUM_LL;
          p += 2;
          continue;
        default:
          goto reject_integer_suffix;
        }
      }
      // l, L
      switch (num->suffix) {
      case NUM_SUFFIX_NONE:
      case NUM_U:
        num->suffix |= NUM_L;
        p++;
        continue;
      default:
        goto reject_integer_suffix;
      }

    default:
    reject_integer_suffix:
      updateContextTo(ctx, p);
      SourceLoc *loc = newSourceLoc(ctx->lineno, ctx->col);
      emitDiagnostic(loc, "Invalid integer suffix '%c'", *p);
    }
  }

  num->x = x;

  num->ty = inferIntegerType(num);
  if (!num->ty) {
    updateContextTo(ctx, begin);
    SourceLoc *loc = newSourceLoc(ctx->lineno, ctx->col);
    emitDiagnostic(loc, "The integer cannot be represented");
  }

  return p - begin;
}

/// Infer the type of an integer type from its radix and suffix. Return the
/// inferred type, or NULL is the integer cannot be represented.
static CType *inferIntegerType(Number *num) {
// Suppress "Case value not in enumerated type 'NumberRadix'" from clang.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"

  // Decimal Constant
  if (num->radix == NUM_DEC) {
    switch (num->suffix) {
    case NUM_SUFFIX_NONE:
      if (num->x <= INT_MAX)
        return newCType(TYPE_INT, TYPE_ATTR_NONE);
      // fallthrough
    case NUM_L:
      if (num->x <= LONG_MAX)
        return newCType(TYPE_LONG, TYPE_ATTR_NONE);
      // fallthrough
    case NUM_LL:
      if (num->x <= LLONG_MAX)
        return newCType(TYPE_LONG_LONG, TYPE_ATTR_NONE);
      return NULL;

    case NUM_U:
      if (num->x <= UINT_MAX)
        return newCType(TYPE_INT, TYPE_ATTR_UNSIGNED);
      // fallthrough
    case NUM_U | NUM_L:
      if (num->x <= ULONG_MAX)
        return newCType(TYPE_LONG, TYPE_ATTR_UNSIGNED);
      // fallthrough
    case NUM_U | NUM_LL:
      if (num->x <= ULLONG_MAX)
        return newCType(TYPE_LONG_LONG, TYPE_ATTR_UNSIGNED);
      return NULL;

    default:
      assert(false && "Unknown integer suffix");
    }
  }

  // Octal or Hexadecimal Constant
  switch (num->suffix) {
  case NUM_SUFFIX_NONE:
  case NUM_U:
    if (!(num->suffix & NUM_U) && num->x <= INT_MAX)
      return newCType(TYPE_INT, TYPE_ATTR_NONE);
    if (num->x <= UINT_MAX)
      return newCType(TYPE_INT, TYPE_ATTR_UNSIGNED);
    // fallthrough
  case NUM_L:
  case NUM_U | NUM_L:
    if (!(num->suffix & NUM_U) && num->x <= LONG_MAX)
      return newCType(TYPE_LONG, TYPE_ATTR_NONE);
    if (num->x <= ULONG_MAX)
      return newCType(TYPE_LONG, TYPE_ATTR_UNSIGNED);
    // fallthrough
  case NUM_LL:
  case NUM_U | NUM_LL:
    if (!(num->suffix & NUM_U) && num->x <= LLONG_MAX)
      return newCType(TYPE_LONG_LONG, TYPE_ATTR_NONE);
    if (num->x <= ULLONG_MAX)
      return newCType(TYPE_LONG_LONG, TYPE_ATTR_UNSIGNED);
    return NULL;

  default:
    assert(false && "Unknown integer suffix");
  }

#pragma clang diagnostic pop
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

  if (num->suffix & NUM_U)
    printf("U");
  if (num->suffix & NUM_L)
    printf("L");
  if (num->suffix & NUM_LL)
    printf("LL");
}
