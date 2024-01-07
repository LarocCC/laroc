#ifndef LAROC_SEMA_DECL_H
#define LAROC_SEMA_DECL_H

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

void printDeclaration(Declaration *decltion, int indent);

#endif
