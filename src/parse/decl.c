#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/token.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "parse/stmt.h"
#include "parse/type.h"
#include "sema/decl.h"
#include "sema/stmt.h"
#include "sema/symbol.h"
#include "sema/type.h"

static int parseDeclarator(ParseCtx *ctx, const Token *begin,
                           Declarator *decltor);

static int parseFunctionDefination(ParseCtx *ctx, const Token *begin,
                                   Declaration *decltion);

static int parseDeclarator(ParseCtx *ctx, const Token *begin,
                           Declarator *decltor) {
  const Token *p = begin;
  int n;

  if (p->kind == TOK_IDENT) {
    decltor->ty = newCType(TYPE_UNTYPED, TYPE_ATTR_NONE);
    decltor->ident = p->ident;
    p++;
  }

  // Function declarator
  if (tokenIsPunct(p, PUNCT_PAREN_L)) {
    p++;

    CType *funcTy = newCType(TYPE_FUNC, TYPE_ATTR_NONE);
    funcTy->func.ret = decltor->ty;
    decltor->ty = funcTy;
    if (tokenIsPunct(p, PUNCT_PAREN_R))
      goto parse_parameter_list_end;

  parse_parameter_list_begin:;
    // Specifier of parameter
    CType *paramSpec = calloc(1, sizeof(CType));
    if ((n = parseSpecifier(p, paramSpec)) == 0) {
      free(paramSpec);
      printf("expect type specifier\n");
      exit(1);
    }
    p += n;

    // Declarator of parameter
    Declarator *paramDecltor = calloc(1, sizeof(Declarator));
    if ((n = parseDeclarator(ctx, p, paramDecltor)) == 0) {
      free(paramDecltor);
      printf("expect declarator\n");
      exit(1);
    }
    p += n;

    paramDecltor->ty = fillUntyped(paramDecltor->ty, paramSpec);
    if (paramDecltor->ty->kind != TYPE_FUNC)
      paramDecltor->ty->attr |= TYPE_ATTR_LVALUE;

    arrput(decltor->ty->func.params, paramDecltor);

    if (tokenIsPunct(p, PUNCT_COMMA)) {
      p++;
      goto parse_parameter_list_begin;
    }

  parse_parameter_list_end:
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

  // Specifier
  CType *spec = calloc(1, sizeof(CType));
  if ((n = parseSpecifier(p, spec)) == 0) {
    free(spec);
    return 0;
  }
  p += n;

  bool allowFuncDef = !ctx->func;

parse_declaration_list_begin:;
  // Declarator
  Declarator *decltor = calloc(1, sizeof(Declarator));
  if ((n = parseDeclarator(ctx, p, decltor)) == 0) {
    free(decltor);
    return 0;
  }
  p += n;
  decltor->ty = fillUntyped(decltor->ty, spec);
  if (decltor->ty->kind != TYPE_FUNC)
    decltor->ty->attr |= TYPE_ATTR_LVALUE;

  if (symTableGetShallow(ctx->symtab, decltor->ident)) {
    printf("symbol %s already exist\n", decltor->ident);
    exit(1);
  }
  Symbol *sym = newSymbol(decltor->ident, decltor->ty);
  symTablePut(ctx->symtab, sym);

  // Function defination
  if (tokenIsPunct(p, PUNCT_BRACE_L)) {
    if (!allowFuncDef) {
      printf("function defination is not allowed here\n");
      exit(1);
    }
    arrput(decltion->decltors, decltor);
    p += parseFunctionDefination(ctx, p, decltion);
    return p - begin;
  }
  allowFuncDef = false;

  // Initializer
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

static int parseFunctionDefination(ParseCtx *ctx, const Token *begin,
                                   Declaration *decltion) {
  assert(tokenIsPunct(begin, PUNCT_BRACE_L));
  Declarator *decltor = decltion->decltors[0];
  const Token *p = begin;

  decltion->funcDef = calloc(1, sizeof(Stmt));
  decltion->labelTable = newSymTable(NULL);
  ctx->symtab = decltion->funcDef->symtab = newSymTable(ctx->symtab);
  ctx->func = decltion;

  for (int i = 0; i < arrlen(decltor->ty->func.params); i++) {
    Declarator *param = decltor->ty->func.params[i];
    if (symTableGetShallow(ctx->symtab, param->ident)) {
      printf("parameter %s already exist\n", param->ident);
      exit(1);
    }
    symTablePut(ctx->symtab, newSymbol(param->ident, param->ty));
  }

  p += parseCmpdStmt(ctx, p, decltion->funcDef);

  ctx->func = NULL;
  ctx->symtab = ctx->symtab->parent;
  return p - begin;
}
