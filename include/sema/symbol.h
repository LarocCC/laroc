#ifndef LAROC_PARSE_SYMTAB_H
#define LAROC_PARSE_SYMTAB_H

#include "typedef.h"

/// A symbol. This may represent a value, or a label in function body.
///
/// TODO: This may also represent a type from typedef.
struct Symbol {
  /// Identifier of the symbol.
  const char *ident;

  /// For a value, type of the symbol.
  CType *ty;
  /// For a value, a IR value with pointer type points to where the value is
  /// stored.
  Value *irValPtr;

  /// For a label, the IR block that the label points to.
  IRBlock *block;
};

Symbol *newSymbol(const char *ident, CType *ty);

/// An entry in symbol table. This is the struct used in stb hash map.
struct SymTableEntry {
  const char *key;
  Symbol *value;
};

/// A symbol table in a scope.
struct SymTable {
  /// The parent symbol table of the current one.
  SymTable *parent;

  SymTableEntry *map;
};

/// Create a new symbol table. Pass NULL to \p parent if the symbol table has no
/// parent.
SymTable *newSymTable(SymTable *parent);

/// Lookup a identifier in \p symtab and its parent recursively.
Symbol *symTableGet(SymTable *symtab, const char *ident);
/// Lookup a identifier in \p symtab but don't lookup its parent if no symbol is
/// found.
Symbol *symTableGetShallow(SymTable *symtab, const char *ident);

void symTablePut(SymTable *symtab, Symbol *sym);

#endif
