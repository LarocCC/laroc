#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/kwd.h"
#include "lex/number.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "sema/expr.h"
#include "sema/symbol.h"
#include "sema/type.h"
#include "util/diag.h"

static int parseCondExpr(ParseCtx *ctx, const Token *begin, Expr **result,
                         Expr *x);

static void setExprCType(ParseCtx *ctx, Expr *expr);

static ExprKind unaryExprKindFromPunct(Punct p);
static ExprKind binaryExprKindFromPunct(Punct p);
static ExprPrecedence exprPrecedence(ExprKind k);

int parseExpr(ParseCtx *ctx, const Token *begin, ExprPrecedence maxPrecedence,
              Expr **result) {
  int n;
  const Token *p = begin;

  // parseExpr() uses shunting yard algorithm to build expression of values,
  // unary operators and binary operators, while other expression including
  // `(x)`, postfix expression (e.g `x[y]`, `x.ident`), `sizeof(T)`, cast
  // expression `T(x)` and conditional expression `x?y:z` are handled recursive
  // descent.
  //
  // Reference: https://www.engr.mun.ca/~theo/Misc/exp_parsing.htm

  // Type checking of expressions is also done in this function.

  // The operand stack of shunting yard algorithm. Each element in the stack is
  // (Expr *) and should point to a valid, or completed expression.
  Expr **valStack = NULL;

  // The operator stack of shunting yard algorithm. Each element in the stack is
  // a (Expr *). The kind of each element is set but operands (x, y) are still
  // NULL.
  Expr **opStack = NULL;

  // At the beginning of parsing, or if we have just seen a binary operator,
  // expectVal is set to true indicating we are expecting next value. Otherwise,
  // if we have just seen a value, expectVal is set to false indicating we are
  // expecting the next binary operator.
  bool expectVal = true;

  // Begin of the loop
parse_expression_begin:

  // Postfix expression
  if (!expectVal && p->kind == TOK_PUNCT) {
    // TODO: x[y]
    // TODO: x(...)

    // s.x s->x
    if (p->punct == PUNCT_DOT || p->punct == PUNCT_ARROW) {
      ExprKind kind = p->punct == PUNCT_DOT ? EXPR_MEMBER : EXPR_PTR_MEMBER;
      Expr *val = newExpr(kind, p->loc);
      p++;

      if (p->kind != TOK_IDENT) {
        emitDiagnostic(p->loc, "Expect identifier");
      }
      val->x = arrpop(valStack);
      val->ident = p->ident;
      p++;
      setExprCType(ctx, val);
      arrput(valStack, val);
      goto parse_expression_begin;
    }

    // x++ x--
    if (p->punct == PUNCT_INCR || p->punct == PUNCT_DECR) {
      ExprKind kind
          = p->punct == PUNCT_INCR ? EXPR_POSTFIX_INCR : EXPR_POSTFIX_DECR;
      Expr *val = newExpr(kind, p->loc);
      p++;
      val->x = arrpop(valStack);
      setExprCType(ctx, val);
      arrput(valStack, val);
      goto parse_expression_begin;
    }
  }

  // Primary expression

  // ident
  if (expectVal && p->kind == TOK_IDENT) {
    Expr *val = newExpr(EXPR_IDENT, p->loc);
    val->ident = p->ident;
    p++;
    setExprCType(ctx, val);
    arrput(valStack, val);
    expectVal = false;
    goto parse_expression_begin;
  }

  // num
  if (expectVal && p->kind == TOK_NUM) {
    Expr *val = newExpr(EXPR_NUM, p->loc);
    val->num = p->num;
    p++;
    setExprCType(ctx, val);
    arrput(valStack, val);
    expectVal = false;
    goto parse_expression_begin;
  }

  // (x)
  if (expectVal && tokenIsPunct(p, PUNCT_PAREN_L)) {
    Expr *val = newExpr(EXPR_INVAL, p->loc);
    p++;

    int n;
    if ((n = parseTypeName(ctx, p, &val->ty)) != 0) {
      p += n;
      if (!tokenIsPunct(p, PUNCT_PAREN_R))
        emitDiagnostic(p->loc, "Expect ')'");
      p++;

      if (tokenIsPunct(p, PUNCT_BRACE_L)) {
        // TODO: (T){...}
      }

      val->kind = EXPR_CAST;
      if ((n = parseExpr(ctx, p, EXPR_PREC_UNARY, &val->x)) == 0)
        emitDiagnostic(p->loc, "Expect expression");
      p += n;
      arrput(valStack, val);
      expectVal = false;
      goto parse_expression_begin;
    }

    p += parseExpr(ctx, p, EXPR_PREC_ALL, &val);
    arrput(valStack, val);

    if (!tokenIsPunct(p, PUNCT_PAREN_R)) {
      emitDiagnostic(p->loc, "Expect ')'");
    }
    p++;

    expectVal = false;
    goto parse_expression_begin;
  }

  // sizeof
  if (expectVal && tokenIsKwd(p, KWD_SIZEOF)) {
    Expr *val = newExpr(EXPR_SIZEOF_VAL, p->loc);
    p++;

    if (tokenIsPunct(p, PUNCT_PAREN_L)) {
      if ((n = parseTypeName(ctx, p + 1, &val->sizeofTy)) != 0) {
        val->kind = EXPR_SIZEOF_TYPE;
        p++;
        p += n;
        if (!tokenIsPunct(p, PUNCT_PAREN_R))
          emitDiagnostic(p->loc, "Expect ')'");
        p++;
        setExprCType(ctx, val);
        arrput(valStack, val);
        expectVal = false;
        goto parse_expression_begin;
      }
    }

    if ((n = parseExpr(ctx, p, EXPR_PREC_ALL, &val->x)) == 0)
      emitDiagnostic(p->loc, "Expect expression");
    p += n;
    setExprCType(ctx, val);
    arrput(valStack, val);
    expectVal = false;
    goto parse_expression_begin;
  }

  // Unary expression
  if (expectVal && p->kind == TOK_PUNCT) {
    ExprKind unaryExprKind = unaryExprKindFromPunct(p->punct);
    if (unaryExprKind != EXPR_INVAL) {
      Expr *val = newExpr(unaryExprKind, p->loc);
      p++;
      p += parseExpr(ctx, p, EXPR_PREC_UNARY, &val->x);
      setExprCType(ctx, val);
      arrput(valStack, val);
      expectVal = false;
      goto parse_expression_begin;
    }
  }

  // Conditional expression x?y:z
  //
  // This will pop x from valStack, parse y and z from the token stream, then
  // push (x?y:z) into valStack.
  if (maxPrecedence >= EXPR_PREC_COND) {
    if (!expectVal && tokenIsPunct(p, PUNCT_COND)) {
      // Pop the operator stack until operator precedence is correct.
      while (arrlen(opStack) > 0) {
        ExprPrecedence stackOpPerc = exprPrecedence(arrlast(opStack)->kind);
        // Using > instead of >= because condition expression's associativity is
        // right-to-left.
        if (stackOpPerc > EXPR_PREC_LOGIC_OR)
          break;

        Expr *stackOp = arrpop(opStack);
        stackOp->y = arrpop(valStack);
        stackOp->x = arrpop(valStack);
        setExprCType(ctx, stackOp);
        arrput(valStack, stackOp);
      }

      assert(arrlen(valStack) > 0);
      Expr *x = arrpop(valStack);
      Expr *cond;
      n = parseCondExpr(ctx, p, &cond, x);
      assert(n > 0);
      p += n;
      arrput(valStack, cond);
    }
  }

  // Binary expressions
  if (!expectVal && p->kind == TOK_PUNCT) {
    // Is there a binary operation matching the current punctator?
    ExprKind binaryExprKind = binaryExprKindFromPunct(p->punct);
    if (binaryExprKind != EXPR_INVAL) {
      // Is the binary operation valid under the constraint from parameter
      // maxPrecedence? Stop parsing if not.
      //
      // FIXME: Assignment expressions' associativity is right-to-left, so
      // should compare the precedence using >= instead of >.
      ExprPrecedence opPrec = exprPrecedence(binaryExprKind);
      if (opPrec > maxPrecedence)
        goto parse_expression_end;

      Expr *op = newExpr(binaryExprKind, p->loc);
      p++;

      // Pop the operator stack until operator precedence is correct.
      while (arrlen(opStack) > 0) {
        ExprPrecedence stackOpPerc = exprPrecedence(arrlast(opStack)->kind);
        if (opPrec < stackOpPerc)
          break;

        Expr *stackOp = arrpop(opStack);
        stackOp->y = arrpop(valStack);
        stackOp->x = arrpop(valStack);
        setExprCType(ctx, stackOp);
        arrput(valStack, stackOp);
      }

      arrput(opStack, op);
      expectVal = true;
      goto parse_expression_begin;
    }
  }

parse_expression_end:
  if (expectVal) {
    emitDiagnostic(p->loc, "Expect expression");
  }

  // Clean the operator stack.
  while (arrlen(opStack) > 0) {
    Expr *stackOp = arrpop(opStack);
    stackOp->y = arrpop(valStack);
    stackOp->x = arrpop(valStack);
    setExprCType(ctx, stackOp);
    arrput(valStack, stackOp);
  }

  assert(arrlen(valStack) == 1);
  *result = arrlast(valStack);
  arrfree(valStack);
  arrfree(opStack);
  return p - begin;
}

/// Parse a condition expression x?y:z, starting from the question mark.
static int parseCondExpr(ParseCtx *ctx, const Token *begin, Expr **result,
                         Expr *x) {
  int n;
  const Token *p = begin;

  assert(tokenIsPunct(p, PUNCT_COND));

  Expr *cond = newExpr(EXPR_COND, p->loc);
  p++;
  *result = cond;
  cond->x = x;

  if ((n = parseExpr(ctx, p, EXPR_PREC_ALL, &cond->y)) == 0) {
    emitDiagnostic(p->loc, "Missing expression");
  }
  p += n;

  if (!tokenIsPunct(p, PUNCT_COLON)) {
    emitDiagnostic(p->loc, "Missing ':'");
  }
  p++;

  if ((n = parseExpr(ctx, p, EXPR_PREC_COND, &cond->z)) == 0) {
    emitDiagnostic(p->loc, "Missing expression");
  }
  p += n;

  setExprCType(ctx, cond);
  return p - begin;
}

static Expr *castExpr(Expr *expr, CType *toTy) {
  if (realTypeSame(expr->ty, toTy))
    return expr;

  Expr *castExpr = newExpr(EXPR_CAST, expr->loc);
  castExpr->ty = toTy;
  castExpr->x = expr;
  return castExpr;
}

static void setExprCType(ParseCtx *ctx, Expr *expr) {
  switch (expr->kind) {
  case EXPR_IDENT:;
    Symbol *sym = symTableGet(ctx->symtab, expr->ident);
    if (!sym) {
      emitDiagnostic(expr->loc, "Cannot determine the type of %s", expr->ident);
    }
    expr->ty = sym->ty;
    return;

  case EXPR_NUM:
    expr->ty = expr->num->ty;
    return;

  case EXPR_MEMBER: {
    if (expr->x->ty->kind != TYPE_STRUCT && expr->x->ty->kind != TYPE_UNION) {
      emitDiagnostic(expr->x->loc, "Expect a struct or a union");
    }
    Symbol *sym = symTableGet(expr->x->ty->struc.symtab, expr->ident);
    if (!sym) {
      printf("struct or union has no member named %s\n", expr->ident);
    }
    expr->ty = sym->ty;
    return;
  }

  case EXPR_PREFIX_INCR:
  case EXPR_PREFIX_DECR:
    if (!typeIsModifiableLvalue(expr->x->ty)) {
      emitDiagnostic(expr->x->loc, "The value is not a modifible lvalue");
    }
    if (typeIsArithmetic(expr->x->ty)) {
      CTypeAttr attr = expr->x->ty->attr & (~TYPE_ATTR_LVALUE);
      expr->ty = newCType(expr->x->ty->kind, attr);
      return;
    }
    break;

  case EXPR_POSTFIX_INCR:
  case EXPR_POSTFIX_DECR:
    if (!typeIsModifiableLvalue(expr->x->ty)) {
      emitDiagnostic(expr->x->loc, "The value is not a modifible lvalue");
    }
    if (typeIsArithmetic(expr->x->ty)) {
      CTypeAttr attr = expr->x->ty->attr & (~TYPE_ATTR_LVALUE);
      expr->ty = newCType(expr->x->ty->kind, attr);
      return;
    }
    break;

  case EXPR_POS:
  case EXPR_NEG:
    if (typeIsArithmetic(expr->x->ty)) {
      expr->ty = integerPromote(expr->x->ty);
      expr->x = castExpr(expr->x, expr->ty);
      return;
    }
    break;

  case EXPR_SIZEOF_TYPE:
  case EXPR_SIZEOF_VAL:
    // C99 6.5.3.4 The sizeof operator (4): The value of the result is
    // implementation-defined, and its type (an unsigned integer type) is
    // size_t, defined in <stddef.h> (and other headers).
    expr->ty = newCType(TYPE_LONG, TYPE_ATTR_UNSIGNED);
    return;

  case EXPR_CAST:
    assert(false && "expr->ty should be set without using this function.");

  case EXPR_MUL:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      expr->x = castExpr(expr->x, expr->ty);
      expr->y = castExpr(expr->y, expr->ty);
      return;
    }
    break;

  case EXPR_ADD:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      expr->x = castExpr(expr->x, expr->ty);
      expr->y = castExpr(expr->y, expr->ty);
      return;
    }
    break;

  case EXPR_SUB:
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      expr->ty = commonRealCType(expr->x->ty, expr->y->ty);
      expr->x = castExpr(expr->x, expr->ty);
      expr->y = castExpr(expr->y, expr->ty);
      return;
    }
    break;

  case EXPR_COND:
    if (!typeIsScarlar(expr->x->ty)) {
      emitDiagnostic(expr->x->loc, "Expression should have a scarlar type");
    }
    if (typeIsArithmetic(expr->y->ty) && typeIsArithmetic(expr->z->ty)) {
      expr->ty = commonRealCType(expr->y->ty, expr->z->ty);
      expr->y = castExpr(expr->y, expr->ty);
      expr->z = castExpr(expr->z, expr->ty);
      return;
    }
    break;

  case EXPR_EQ_ASSIGN:
    if (!typeIsModifiableLvalue(expr->x->ty)) {
      emitDiagnostic(expr->x->loc, "Expression is not assignable");
    }
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      CTypeAttr attr = expr->x->ty->attr & ~TYPE_ATTR_LVALUE;
      expr->ty = newCType(expr->x->ty->kind, attr);
      expr->y = castExpr(expr->y, expr->ty);
      return;
    }
    break;

  case EXPR_ADD_EQ:
  case EXPR_SUB_EQ:
    if (!typeIsModifiableLvalue(expr->x->ty)) {
      emitDiagnostic(expr->x->loc, "Expression is not assignable");
    }
    if (typeIsArithmetic(expr->x->ty) && typeIsArithmetic(expr->y->ty)) {
      CTypeAttr attr = expr->x->ty->attr & ~TYPE_ATTR_LVALUE;
      expr->ty = newCType(expr->x->ty->kind, attr);
      expr->y = castExpr(expr->y, expr->ty);
      return;
    }
    break;

  case EXPR_COMMA:
    // FIXME: C99 6.5.17 Comma operator (2): A comma operator does not yield an
    // lvalue.
    expr->ty = expr->y->ty;
    return;

  default:
    break;
  }

  emitDiagnostic(expr->loc, "Cannot determine the type of the expression");
}

static ExprKind unaryExprKindFromPunct(Punct p) {
  switch (p) {
  case PUNCT_INCR: // ++
    return EXPR_PREFIX_INCR;
  case PUNCT_DECR: // --
    return EXPR_PREFIX_DECR;
  case PUNCT_AMP: // &
    return EXPR_ADDR;
  case PUNCT_STAR: // *
    return EXPR_INDIR;
  case PUNCT_PLUS: // +
    return EXPR_POS;
  case PUNCT_MINUS: // -
    return EXPR_NEG;
  case PUNCT_BIT_NOT: // ~
    return EXPR_BIT_NOT;
  case PUNCT_LOGIC_NOT: // !
    return EXPR_LOGIC_NOT;
  default:
    return EXPR_INVAL;
  }
}

static ExprKind binaryExprKindFromPunct(Punct p) {
  switch (p) {
  case PUNCT_STAR:
    return EXPR_MUL;
  case PUNCT_DIV:
    return EXPR_DIV;
  case PUNCT_MOD:
    return EXPR_MOD;

  case PUNCT_PLUS:
    return EXPR_ADD;
  case PUNCT_MINUS:
    return EXPR_SUB;

  case PUNCT_SHIFT_L:
    return EXPR_SHIFT_L;
  case PUNCT_SHIFT_R:
    return EXPR_SHIFT_R;

  case PUNCT_LT:
    return EXPR_LT;
  case PUNCT_GT:
    return EXPR_GT;
  case PUNCT_LEQ:
    return EXPR_LEQ;
  case PUNCT_GEQ:
    return EXPR_GEQ;

  case PUNCT_EQ_CMP:
    return EXPR_EQ_CMP;
  case PUNCT_NEQ:
    return EXPR_NEQ;

  case PUNCT_AMP:
    return EXPR_BIT_AND;

  case PUNCT_BIT_XOR:
    return EXPR_BIT_XOR;

  case PUNCT_BIT_OR:
    return EXPR_BIT_OR;

  case PUNCT_LOGIC_AND:
    return EXPR_LOGIC_AND;

  case PUNCT_LOGIC_OR:
    return EXPR_LOGIC_OR;

  case PUNCT_EQ_ASSIGN:
    return EXPR_EQ_ASSIGN;
  case PUNCT_MUL_EQ:
    return EXPR_MUL_EQ;
  case PUNCT_DIV_EQ:
    return EXPR_DIV_EQ;
  case PUNCT_MOD_EQ:
    return EXPR_MOD_EQ;
  case PUNCT_ADD_EQ:
    return EXPR_ADD_EQ;
  case PUNCT_SUB_EQ:
    return EXPR_SUB_EQ;
  case PUNCT_SHIFT_L_EQ:
    return EXPR_SHIFT_L_EQ;
  case PUNCT_SHIFT_R_EQ:
    return EXPR_SHIFT_R_EQ;
  case PUNCT_BIT_AND_EQ:
    return EXPR_BIT_AND_EQ;
  case PUNCT_BIT_XOR_EQ:
    return EXPR_BIT_XOR_EQ;
  case PUNCT_BIT_OR_EQ:
    return EXPR_BIT_OR_EQ;

  case PUNCT_COMMA:
    return EXPR_COMMA;

  default:
    return EXPR_INVAL;
  }
}

static ExprPrecedence exprPrecedence(ExprKind k) {
  switch (k) {
  case EXPR_MUL:
  case EXPR_DIV:
  case EXPR_MOD:
    return EXPR_PREC_MUL;

  case EXPR_ADD:
  case EXPR_SUB:
    return EXPR_PREC_ADD;

  case EXPR_SHIFT_L:
  case EXPR_SHIFT_R:
    return EXPR_PREC_SHIFT;

  case EXPR_LT:
  case EXPR_GT:
  case EXPR_LEQ:
  case EXPR_GEQ:
    return EXPR_PREC_REL;

  case EXPR_EQ_CMP:
  case EXPR_NEQ:
    return EXPR_PREC_EQ;

  case EXPR_BIT_AND:
    return EXPR_PREC_BIT_AND;

  case EXPR_BIT_XOR:
    return EXPR_PREC_BIT_XOR;

  case EXPR_BIT_OR:
    return EXPR_PREC_BIT_OR;

  case EXPR_LOGIC_AND:
    return EXPR_PREC_LOGIC_AND;

  case EXPR_LOGIC_OR:
    return EXPR_PREC_LOGIC_OR;

  case EXPR_EQ_ASSIGN:
  case EXPR_MUL_EQ:
  case EXPR_DIV_EQ:
  case EXPR_MOD_EQ:
  case EXPR_ADD_EQ:
  case EXPR_SUB_EQ:
  case EXPR_SHIFT_L_EQ:
  case EXPR_SHIFT_R_EQ:
  case EXPR_BIT_AND_EQ:
  case EXPR_BIT_XOR_EQ:
  case EXPR_BIT_OR_EQ:
    return EXPR_PREC_ASSIGN;

  case EXPR_COMMA:
    return EXPR_PREC_ALL;

  default:
    return 0;
  }
}
