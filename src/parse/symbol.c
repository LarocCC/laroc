#include <assert.h>
#include <stdlib.h>

#include "stb_ds.h"

#include "typedef.h"
#include "parse/decl.h"
#include "parse/symbol.h"

Symbol *newSymbol(const char *ident, CType *ty) {
  Symbol *sym = calloc(1, sizeof(Symbol));
  sym->ident = ident;
  sym->ty = ty;
  return sym;
}

SymTable *newSymTable(SymTable *parent) {
  SymTable *symtab = calloc(1, sizeof(SymTable));
  symtab->parent = parent;
  shdefault(symtab->map, NULL);
  return symtab;
}

Symbol *symTableGet(SymTable *symtab, const char *ident) {
  while (symtab != NULL) {
    Symbol *e = symTableGetShallow(symtab, ident);
    if (e != NULL)
      return e;
    symtab = symtab->parent;
  }
  return NULL;
}

Symbol *symTableGetShallow(SymTable *symtab, const char *ident) {
  return shget(symtab->map, ident);
}

void symTablePut(SymTable *symtab, Symbol *sym) {
  assert(symTableGetShallow(symtab, sym->ident) == NULL);
  SymTableEntry e = {
      .key = sym->ident,
      .value = sym,
  };
  shputs(symtab->map, e);
}
