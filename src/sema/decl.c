#include <stdio.h>

#include "stb/stb_ds.h"

#include "sema/decl.h"
#include "sema/expr.h"
#include "sema/stmt.h"
#include "sema/type.h"

void printDeclarator(Declarator *declator, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");
  printf("Declarator ident='%s'\n", declator->ident);
  printCType(declator->ty, indent + 1);
  if (declator->init)
    printExpr(declator->init, indent + 1);
}

void printDeclaration(Declaration *decltion, int indent) {
  for (int i = 0; i < indent; i++)
    printf("  ");

  if (decltion->funcDef) {
    printf("FunctionDefination\n");
    printDeclarator(decltion->decltors[0], indent + 1);
    printStmt(decltion->funcDef, indent + 1);
    return;
  }

  printf("Declaration\n");
  for (int i = 0; i < arrlen(decltion->decltors); i++)
    printDeclarator(decltion->decltors[i], indent + 1);
}
