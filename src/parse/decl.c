#include <stdio.h>
#include <stdlib.h>

#include "stb_ds.h"

#include "typedef.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/parse.h"
#include "parse/expr.h"
#include "parse/stmt.h"
#include "parse/type.h"

int parseDeclarator(ParseCtx *ctx, const Token *begin, Declarator *decltor) {
  const Token *p = begin;

  if (p->kind == TOK_IDENT) {
    decltor->ty = newType(TYPE_UNTYPED);
    decltor->ident = p->ident;
    p++;
  }

  if (tokenIsPunct(p, PUNCT_PAREN_L)) {
    p++;

    Type *funcTy = newType(TYPE_FUNC);
    funcTy->func.ret = decltor->ty;
    decltor->ty = funcTy;

    if (!tokenIsPunct(p, PUNCT_PAREN_R)) {
      printf("expect right paren\n");
      exit(1);
    }
    p++;

    goto parse_declarator_end;
  }

parse_declarator_end:
  return p - begin;
}

int parseDeclaration(ParseCtx *ctx, const Token *begin, Declaration *decltion) {
  const Token *p = begin;
  int n;

  Type *spec = calloc(1, sizeof(Type));
  if ((n = parseSpecifier(p, spec)) == 0) {
    free(spec);
    return 0;
  }
  p += n;

  bool allowFuncDef = ctx->func == NULL;

parse_declaration_list_begin:;
  Declarator *decltor = calloc(1, sizeof(Declarator));
  if ((n = parseDeclarator(ctx, p, decltor)) == 0) {
    free(decltor);
    return 0;
  }
  p += n;
  decltor->ty = fillUntyped(decltor->ty, spec);

  if (tokenIsPunct(p, PUNCT_BRACE_L)) {
    if (!allowFuncDef) {
      printf("function defination is not allowed here\n");
      exit(1);
    }
    decltion->funcDef = calloc(1, sizeof(Stmt));
    ctx->func = decltion;
    p += parseCmpdStmt(ctx, p, decltion->funcDef);
    arrput(decltion->decltors, decltor);
    ctx->func = NULL;
    return p - begin;
  }
  allowFuncDef = false;

  if (tokenIsPunct(p, PUNCT_EQ_ASSIGN)) {
    p++;
    if ((n = parseExpr(ctx, p, EXPR_PREC_ASSIGN, &decltor->init)) == 0) {
      printf("expect expression\n");
      exit(1);
    }
    p += n;
  }

  arrput(decltion->decltors, decltor);

  if (tokenIsPunct(p, PUNCT_COMMA)) {
    p++;
    goto parse_declaration_list_begin;
  }

  if (!tokenIsPunct(p, PUNCT_SEMICOLON)) {
    printf("missing semicolon\n");
    exit(1);
  }
  p++;
  return p - begin;
}
