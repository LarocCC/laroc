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

  IRBlock *block;
  Value *irValPtr;
};

Symbol *newSymbol(const char *ident, CType *ty);

struct SymTableEntry {
  const char *key;
  Symbol *value;
};

struct SymTable {
  SymTable *parent;

  SymTableEntry *map;
};

SymTable *newSymTable(SymTable *parent);

Symbol *symTableGet(SymTable *symtab, const char *ident);
Symbol *symTableGetShallow(SymTable *symtab, const char *ident);

void symTablePut(SymTable *symtab, Symbol *sym);

#endif
