#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "typedef.h"
#include "lex/lex.h"
#include "sema/number.h"
#include "sema/stmt.h"
#include "sema/type.h"
#include "util/diag.h"

typedef enum {
  SUFFIX_NONE = 0,
  SUFFIX_U = 1 << 1,
  SUFFIX_L = 1 << 2,
  SUFFIX_LL = 1 << 3,
} Suffix;

static CType *inferIntegerType(int radix, Suffix suffix, uint64_t i);
static int scanFloat(LexCtx *ctx, const char *begin, const char *end,
                     Number *num);

int scanNumber(LexCtx *ctx, const char *begin, const char *end, Number *num) {
  const char *p = begin;

  int radix = 10;
  if (*p == '0') {
    if (p + 1 < end && (*(p + 1) == 'x' || *(p + 1) == 'X')) {
      radix = 16;
    } else {
      radix = 8;
    }
  }

  num->kind = NUM_INT;

  char *num_end;
  num->i = strtoull(p, &num_end, radix);
  if (errno == ERANGE) {
    updateContextTo(ctx, p);
    SourceLoc *loc = newSourceLoc(ctx->lineno, ctx->col);
    emitDiagnostic(loc, "The integer cannot be represented");
  }
  assert(num_end != p);
  p = num_end;

  Suffix suffix = SUFFIX_NONE;

  while (p < end && (isalnum(*p) || *p == '.')) {
    switch (*p) {
    case '.':
    case 'e':
    case 'E':
    case 'p':
    case 'P':
      return scanFloat(ctx, begin, end, num);

    case 'u':
    case 'U':
      switch (suffix) {
      case SUFFIX_NONE:
      case SUFFIX_L:
      case SUFFIX_LL:
        suffix |= SUFFIX_U;
        p++;
        continue;
      default:
        goto reject_integer_suffix;
      }

    case 'l':
    case 'L':
      // ll, LL
      if (p + 1 < end && *(p + 1) == *p) {
        switch (suffix) {
        case SUFFIX_NONE:
        case SUFFIX_U:
          suffix |= SUFFIX_LL;
          p += 2;
          continue;
        default:
          goto reject_integer_suffix;
        }
      }
      // l, L
      switch (suffix) {
      case SUFFIX_NONE:
      case SUFFIX_U:
        suffix |= SUFFIX_L;
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

  num->ty = inferIntegerType(radix, suffix, num->i);
  if (!num->ty) {
    updateContextTo(ctx, begin);
    SourceLoc *loc = newSourceLoc(ctx->lineno, ctx->col);
    emitDiagnostic(loc, "The integer cannot be represented");
  }

  return p - begin;
}

/// Infer the type of an integer type from its radix and suffix. Return the
/// inferred type, or NULL is the integer cannot be represented.
static CType *inferIntegerType(int radix, Suffix suffix, uint64_t i) {
// Suppress "Case value not in enumerated type 'Suffix'" from clang.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"

  // Decimal Constant
  if (radix == 10) {
    switch (suffix) {
    case SUFFIX_NONE:
      if (i <= INT_MAX)
        return newCType(TYPE_INT, TYPE_ATTR_NONE);
      // fallthrough
    case SUFFIX_L:
      if (i <= LONG_MAX)
        return newCType(TYPE_LONG, TYPE_ATTR_NONE);
      // fallthrough
    case SUFFIX_LL:
      if (i <= LLONG_MAX)
        return newCType(TYPE_LONG_LONG, TYPE_ATTR_NONE);
      return NULL;

    case SUFFIX_U:
      if (i <= UINT_MAX)
        return newCType(TYPE_INT, TYPE_ATTR_UNSIGNED);
      // fallthrough
    case SUFFIX_U | SUFFIX_L:
      if (i <= ULONG_MAX)
        return newCType(TYPE_LONG, TYPE_ATTR_UNSIGNED);
      // fallthrough
    case SUFFIX_U | SUFFIX_LL:
      if (i <= ULLONG_MAX)
        return newCType(TYPE_LONG_LONG, TYPE_ATTR_UNSIGNED);
      return NULL;

    default:
      assert(false && "Unknown integer suffix");
    }
  }

  // Octal or Hexadecimal Constant
  switch (suffix) {
  case SUFFIX_NONE:
  case SUFFIX_U:
    if (!(suffix & SUFFIX_U) && i <= INT_MAX)
      return newCType(TYPE_INT, TYPE_ATTR_NONE);
    if (i <= UINT_MAX)
      return newCType(TYPE_INT, TYPE_ATTR_UNSIGNED);
    // fallthrough
  case SUFFIX_L:
  case SUFFIX_U | SUFFIX_L:
    if (!(suffix & SUFFIX_U) && i <= LONG_MAX)
      return newCType(TYPE_LONG, TYPE_ATTR_NONE);
    if (i <= ULONG_MAX)
      return newCType(TYPE_LONG, TYPE_ATTR_UNSIGNED);
    // fallthrough
  case SUFFIX_LL:
  case SUFFIX_U | SUFFIX_LL:
    if (!(suffix & SUFFIX_U) && i <= LLONG_MAX)
      return newCType(TYPE_LONG_LONG, TYPE_ATTR_NONE);
    if (i <= ULLONG_MAX)
      return newCType(TYPE_LONG_LONG, TYPE_ATTR_UNSIGNED);
    return NULL;

  default:
    assert(false && "Unknown integer suffix");
  }

#pragma clang diagnostic pop
}

static int scanFloat(LexCtx *ctx, const char *begin, const char *end,
                     Number *num) {
  const char *p = begin;
  num->kind = NUM_FLOAT;

  char *num_end;
  num->f = strtold(p, &num_end);
  if (errno == ERANGE) {
    updateContextTo(ctx, p);
    SourceLoc *loc = newSourceLoc(ctx->lineno, ctx->col);
    emitDiagnostic(loc, "The floating-point number cannot be represented");
  }
  assert(num_end != p);
  p = num_end;

  if (p < end && isalnum(*p)) {
    switch (*p) {
    case 'f':
    case 'F':
      num->ty = newCType(TYPE_FLOAT, TYPE_ATTR_NONE);
      p++;
      break;

    case 'l':
    case 'L':
      num->ty = newCType(TYPE_LONG_DOUBLE, TYPE_ATTR_NONE);
      p++;
      break;
    }
  }

  if (p < end && isalnum(*p)) {
    updateContextTo(ctx, p);
    SourceLoc *loc = newSourceLoc(ctx->lineno, ctx->col);
    emitDiagnostic(loc, "Unknown character '%c'", *p);
  }

  return p - begin;
}

void printNumber(const Number *num) {
  switch (num->kind) {
  case NUM_INT:
    printf("Integer %lu (%#lx)", num->i, num->i);
    return;

  case NUM_FLOAT:
    printf("Float %Lg (%La)", num->f, num->f);
    return;

  default:
    assert(false && "Unreachable");
  }
}
