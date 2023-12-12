#ifndef LAROC_PARSE_DECL_H
#define LAROC_PARSE_DECL_H

#include <stdbool.h>

#include "typedef.h"

struct Declarator {
  Type *ty;
  const char *ident;
  Stmt *funcDef;
};

int parseDeclarator(const Token *begin, Declarator *decltor);

struct Declaration {
  bool isFuncDef;
  Declarator **decltors;
};

int parseDeclaration(const Token *begin, Declaration *decltion);

#endif
