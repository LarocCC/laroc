#ifndef LAROC_SEMA_DECL_H
#define LAROC_SEMA_DECL_H

#include <stdbool.h>

#include "typedef.h"

/// A declared identifier in C with optional initializer.
struct Declarator {
  /// Type of the declared identifier.
  CType *ty;

  // The offset of the value, if this declarator in inside a struct of a union.
  int offset;

  const char *ident;

  // Expression of the initializer, or NULL if initialzier is not present.
  Expr *init;

  // Location of the identifier.
  SourceLoc *loc;
};

void printDeclarator(Declarator *declator, int indent, bool inStruct);

/// A C declaration or function defination.
struct Declaration {
  /// All declared identifiers of a declaration. Or for a function defination,
  /// the only one element in the array is the function.
  Declarator **decltors;

  /// NULL for a declaration, or a componud statement representing function body
  /// for a function defination.
  Stmt *funcDef;
  /// All labels in the function.
  SymTable *labelTable;
};

void printDeclaration(Declaration *decltion, int indent, bool inStruct);

#endif
