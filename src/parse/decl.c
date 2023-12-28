#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/punct.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/expr.h"
#include "parse/parse.h"
#include "parse/stmt.h"
#include "parse/symbol.h"
#include "parse/type.h"

int parseDeclarator(ParseCtx *ctx, const Token *begin, Declarator *decltor) {
  const Token *p = begin;
  int n;

  if (p->kind == TOK_IDENT) {
    decltor->ty = newCType(TYPE_UNTYPED);
    decltor->ident = p->ident;
    p++;
  }

  if (tokenIsPunct(p, PUNCT_PAREN_L)) {
    p++;

    CType *funcTy = newCType(TYPE_FUNC);
    funcTy->func.ret = decltor->ty;
    decltor->ty = funcTy;
    if (tokenIsPunct(p, PUNCT_PAREN_R))
      goto parse_parameter_list_end;

  parse_parameter_list_begin:;
    CType *paramSpec = calloc(1, sizeof(CType));
    if ((n = parseSpecifier(p, paramSpec)) == 0) {
      free(paramSpec);
      printf("expect type specifier\n");
      exit(1);
    }
    p += n;

    Declarator *paramDecltor = calloc(1, sizeof(Declarator));
    if ((n = parseDeclarator(ctx, p, paramDecltor)) == 0) {
      free(paramDecltor);
      printf("expect declarator\n");
      exit(1);
    }
    p += n;
    paramDecltor->ty = fillUntyped(paramDecltor->ty, paramSpec);
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

void printDeclarator(Declarator *declator, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");
  printf("Declarator ident='%s'\n", declator->ident);
  printCType(declator->ty, indent + 1);
  if (declator->init != NULL)
    printExpr(declator->init, indent + 1);
}

int parseDeclaration(ParseCtx *ctx, const Token *begin, Declaration *decltion) {
  const Token *p = begin;
  int n;

  CType *spec = calloc(1, sizeof(CType));
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

  if (symTableGetShallow(ctx->symtab, decltor->ident) != NULL) {
    printf("symbol %s already exist\n", decltor->ident);
    exit(1);
  }
  Symbol *sym = newSymbol(decltor->ident, decltor->ty);
  symTablePut(ctx->symtab, sym);

  if (tokenIsPunct(p, PUNCT_BRACE_L)) {
    if (!allowFuncDef) {
      printf("function defination is not allowed here\n");
      exit(1);
    }
    arrput(decltion->decltors, decltor);
    decltion->funcDef = calloc(1, sizeof(Stmt));
    ctx->symtab = decltion->funcDef->symtab = newSymTable(ctx->symtab);
    ctx->func = decltion;

    for (int i = 0; i < arrlen(decltor->ty->func.params); i++) {
      Declarator *param = decltor->ty->func.params[i];
      if (symTableGetShallow(ctx->symtab, param->ident) != NULL) {
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

void printDeclaration(Declaration *decltion, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");

  if (decltion->funcDef != NULL) {
    printf("FunctionDefination\n");
    printDeclarator(decltion->decltors[0], indent + 1);
    printStmt(decltion->funcDef, indent + 1);
    return;
  }

  printf("Declaration\n");
  for (int i = 0; i < arrlen(decltion->decltors); i++)
    printDeclarator(decltion->decltors[i], indent + 1);
}
