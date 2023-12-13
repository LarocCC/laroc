#ifndef LAROC_PARSE_EXPR_H
#define LAROC_PARSE_EXPR_H

#include <stdint.h>
#include <stdlib.h>

#include "typedef.h"

enum ExprKind {
  EXPR_INVAL,

  // C99 (6.5.1) primary-expression
  EXPR_IDENT, // ident
  EXPR_NUM,   // num

  // C99 (6.5.5) multiplicative-expression
  EXPR_MUL, // x*y
  EXPR_DIV, // x/y
  EXPR_MOD, // x%y

  // C99 (6.5.6) additive-expression
  EXPR_ADD, // x+y
  EXPR_SUB, // x-y

  // C99 (6.5.7) shift-expression
  EXPR_SHIFT_L, // x<<y
  EXPR_SHIFT_R, // x>>y

  // C99 (6.5.8) relational-expression
  EXPR_LT,  // x<y
  EXPR_GT,  // x>y
  EXPR_LEQ, // x<=y
  EXPR_GEQ, // x>=y

  // C99 (6.5.9) equality-expression
  EXPR_EQ_CMP, // x==y
  EXPR_NEQ,    // x!=y

  // C99 (6.5.10) AND-expression
  EXPR_BIT_AND, // x&y

  // C99 (6.5.11) exclusive-OR-expression
  EXPR_BIT_XOR, // x^y

  // C99 (6.5.12) inclusive-OR-expression
  EXPR_BIT_OR, // x|y

  // C99 (6.5.13) logical-AND-expression
  EXPR_LOGIC_AND, // x&&y

  // C99 (6.5.14) logical-OR-expression
  EXPR_LOGIC_OR, // x||y

  // C99 (6.5.16) assignment-expression
  EXPR_EQ_ASSIGN,  // x=y
  EXPR_MUL_EQ,     // x*=y
  EXPR_DIV_EQ,     // x/=y
  EXPR_MOD_EQ,     // x%=y
  EXPR_ADD_EQ,     // x+=y
  EXPR_SUB_EQ,     // x-=y
  EXPR_SHIFT_L_EQ, // x<<=y
  EXPR_SHIFT_R_EQ, // x>>=y
  EXPR_BIT_AND_EQ, // x&=y
  EXPR_BIT_XOR_EQ, // x^=y
  EXPR_BIT_OR_EQ,  // x|=y

  // C99 (6.5.17) expression
  EXPR_COMMA, // x,y
};

struct Expr {
  ExprKind kind;

  const char *ident;
  Number *num;

  Expr *x, *y;
};

Expr *newExpr(ExprKind kind);

/// Parse a expression start at \p begin. Store a (Expr *) to \p result and
/// return the tokens consumed. This function will not parse operators with
/// precedence greater than \p maxPrecedence.
int parseExpr(ParseCtx *ctx, const Token *begin, ExprPrecedence maxPrecedence, Expr **result);

/// Value from https://en.cppreference.com/w/c/language/operator_precedence,
/// times 10.
enum ExprPrecedence {
  // C99 (6.5.1) primary-expression
  EXPR_PREC_PRIMARY = 0,
  // C99 (6.5.2) postfix-expression
  EXPR_PREC_POSTFIX = 10,
  // C99 (6.5.3) unary-expression
  EXPR_PREC_UNARY = 20,
  // C99 (6.5.5) multiplicative-expression
  EXPR_PREC_MUL = 30,
  // C99 (6.5.6) additive-expression
  EXPR_PREC_ADD = 40,
  // C99 (6.5.7) shift-expression
  EXPR_PREC_SHIFT = 50,
  // C99 (6.5.8) relational-expression
  EXPR_PREC_REL = 60,
  // C99 (6.5.9) equality-expression
  EXPR_PREC_EQ = 70,
  // C99 (6.5.10) AND-expression
  EXPR_PREC_BIT_AND = 80,
  // C99 (6.5.11) exclusive-OR-expression
  EXPR_PREC_BIT_XOR = 90,
  // C99 (6.5.12) inclusive-OR-expression
  EXPR_PREC_BIT_OR = 100,
  // C99 (6.5.13) logical-AND-expression
  EXPR_PREC_LOGIC_AND = 110,
  // C99 (6.5.14) logical-OR-expression
  EXPR_PREC_LOGIC_OR = 120,
  // C99 (6.5.15) conditional-expression
  EXPR_PREC_COND = 130,
  // C99 (6.5.16) assignment-expression
  EXPR_PREC_ASSIGN = 140,
  // C99 (6.5.17) expression
  EXPR_PREC_ALL = 150,
};

#endif
