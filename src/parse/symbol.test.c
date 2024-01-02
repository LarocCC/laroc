#include <assert.h>
#include <stdlib.h>

#include "parse/symbol.h"
#include "parse/type.h"

int main() {
  SymTable *symtab1 = newSymTable(NULL);
  Symbol a1 = {
      .ident = "a",
      .ty = newCType(TYPE_INT, TYPE_ATTR_LVALUE),
  };
  Symbol b = {
      .ident = "b",
      .ty = newCType(TYPE_INT, TYPE_ATTR_LVALUE),
  };
  symTablePut(symtab1, &a1);
  symTablePut(symtab1, &b);
  assert(symTableGet(symtab1, "a") == &a1);
  assert(symTableGet(symtab1, "b") == &b);

  SymTable *symtab2 = newSymTable(symtab1);
  Symbol a2 = {
      .ident = "a",
      .ty = newCType(TYPE_INT, TYPE_ATTR_LVALUE),
  };
  symTablePut(symtab2, &a2);
  assert(symTableGet(symtab2, "a") == &a2);
  assert(symTableGet(symtab2, "b") == &b);
  assert(symTableGetShallow(symtab2, "b") == NULL);

  return 0;
}
