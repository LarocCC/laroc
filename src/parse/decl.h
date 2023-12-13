#ifndef LAROC_PARSE_DECL_H
#define LAROC_PARSE_DECL_H

#include <stdbool.h>

#include "typedef.h"

struct Declarator {
  Type *ty;
  const char *ident;
  Expr *init;
};

int parseDeclarator(ParseCtx *ctx, const Token *begin, Declarator *decltor);

struct Declaration {
  Declarator **decltors;
  Stmt *funcDef;
  SymTable *funcSymtab;
};

int parseDeclaration(ParseCtx *ctx, const Token *begin, Declaration *decltion);

#endif
