#ifndef LAROC_SEMA_DECL_H
#define LAROC_SEMA_DECL_H

#include "typedef.h"

/// A declared identifier in C with optional initializer.
struct Declarator {
  /// Type of the declared identifier.
  CType *ty;

  const char *ident;

  // Expression of the initializer, or NULL if initialzier is not present.
  Expr *init;
};

void printDeclarator(Declarator *declator, int indent);

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

void printDeclaration(Declaration *decltion, int indent);

#endif
