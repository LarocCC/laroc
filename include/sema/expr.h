#ifndef LAROC_SEMA_EXPR_H
#define LAROC_SEMA_EXPR_H

#include "typedef.h"

typedef enum ExprKind {
  // Invalid expression
  EXPR_INVAL,

  // C99 (6.5.1) primary-expression
  //
  // TODO: Add character constant and string literal.
  EXPR_IDENT, // ident
  EXPR_NUM,   // num

  // C99 (6.5.2) postfix-expression
  //
  // TODO: x[y]
  // TODO: x(...)
  // TODO: x.ident
  // TODO: x->ident
  // TODO: x++
  // TODO: x--
  // TODO: (T){...}

  // C99 (6.5.3) unary-expression
  //
  // TODO: ++x
  // TODO: --x
  // TODO: &x
  // TODO: *x
  // TODO: +x
  // TODO: -x
  // TODO: ~x
  // TODO: !x
  // TODO: sizeof x
  // TODO: sizeof(T)

  // C99 (6.5.4) cast-expression
  //
  // TODO: (T)x

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

  // C99 (6.5.15) conditional-expression
  //
  // TODO: x?y:z

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
} ExprKind;

/// A C expression.
///
/// TODO: Use unions to save memory.
struct Expr {
  ExprKind kind;

  /// Type of the expression.
  CType *ty;

  const char *ident;
  Number *num;

  Expr *x, *y;
};

Expr *newExpr(ExprKind kind);

void printExpr(Expr *expr, int indent);

#endif
