#include <stdio.h>

#include "stb/stb_ds.h"

#include "sema/decl.h"
#include "sema/expr.h"
#include "sema/stmt.h"
#include "sema/type.h"
#include "util/diag.h"

void printDeclarator(Declarator *declator, int indent, bool inStruct) {
  for (int i = 0; i < indent; i++)
    printf("  ");
  printf("Declarator [%d:%d] ident='%s'", declator->loc->lineno,
         declator->loc->col, declator->ident);
  if (inStruct) {
    printf(" offset=%d\n", declator->offset);
  } else {
    printf("\n");
  }
  printCType(declator->ty, indent + 1);
  if (declator->init)
    printExpr(declator->init, indent + 1);
}

void printDeclaration(Declaration *decltion, int indent, bool inStruct) {
  for (int i = 0; i < indent; i++)
    printf("  ");

  if (decltion->funcDef) {
    printf("FunctionDefination\n");
    printDeclarator(decltion->decltors[0], indent + 1, inStruct);
    printStmt(decltion->funcDef, indent + 1);
    return;
  }

  printf("Declaration\n");
  for (int i = 0; i < arrlen(decltion->decltors); i++)
    printDeclarator(decltion->decltors[i], indent + 1, inStruct);
}
