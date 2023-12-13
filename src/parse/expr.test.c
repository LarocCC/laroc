#include <assert.h>
#include <string.h>

#include "lex/lex.h"
#include "parse/expr.h"

int main() {
  const char *source;
  Token *tokens;
  Expr *expr;

  source = "laroc";
  tokens = lex(source, strlen(source));
  assert(parseExpr(tokens, EXPR_PREC_ALL, &expr) == 1);
  assert(expr->kind == EXPR_IDENT);
  assert(strcmp(expr->ident, "laroc") == 0);

  source = "123";
  tokens = lex(source, strlen(source));
  assert(parseExpr(tokens, EXPR_PREC_ALL, &expr) == 1);
  assert(expr->kind == EXPR_NUM);
  assert(expr->num == 123);

  source = "a + b * c";
  tokens = lex(source, strlen(source));
  assert(parseExpr(tokens, EXPR_PREC_ALL, &expr) == 5);
  assert(expr->kind == EXPR_ADD);
  assert(expr->x->kind == EXPR_IDENT);
  assert(strcmp(expr->x->ident, "a") == 0);
  assert(expr->y->kind == EXPR_MUL);
  assert(expr->y->x->kind == EXPR_IDENT);
  assert(strcmp(expr->y->x->ident, "b") == 0);
  assert(expr->y->y->kind == EXPR_IDENT);
  assert(strcmp(expr->y->y->ident, "c") == 0);

  source = "a * b + c";
  tokens = lex(source, strlen(source));
  assert(parseExpr(tokens, EXPR_PREC_ALL, &expr) == 5);
  assert(expr->kind == EXPR_ADD);
  assert(expr->x->kind == EXPR_MUL);
  assert(expr->x->x->kind == EXPR_IDENT);
  assert(strcmp(expr->x->x->ident, "a") == 0);
  assert(expr->x->y->kind == EXPR_IDENT);
  assert(strcmp(expr->x->y->ident, "b") == 0);
  assert(expr->y->kind == EXPR_IDENT);
  assert(strcmp(expr->y->ident, "c") == 0);

  source = "a, b";
  tokens = lex(source, strlen(source));
  assert(parseExpr(tokens, EXPR_PREC_ASSIGN, &expr) == 1);

  return 0;
}
