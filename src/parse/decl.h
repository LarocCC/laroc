#ifndef LAROC_PARSE_DECL_H
#define LAROC_PARSE_DECL_H

#include <stdbool.h>

#include "typedef.h"

struct Declarator {
  CType *ty;
  const char *ident;
  Expr *init;
};

void printDeclarator(Declarator *declator, int indent);

struct Declaration {
  Declarator **decltors;

  Stmt *funcDef;
  SymTable *labelTable;
};

int parseDeclaration(ParseCtx *ctx, const Token *begin, Declaration *decltion);

void printDeclaration(Declaration *decltion, int indent);

#endif
