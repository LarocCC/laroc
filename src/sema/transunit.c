#include "stb/stb_ds.h"

#include "sema/decl.h"
#include "sema/symbol.h"
#include "sema/transunit.h"

TranslationUnit *newTranslationUnit() {
  TranslationUnit *unit = calloc(1, sizeof(TranslationUnit));
  unit->symtab = newSymTable(NULL);
  return unit;
}

void printTranslationUnit(TranslationUnit *unit) {
  for (int i = 0; i < arrlen(unit->decltions); i++)
    printDeclaration(unit->decltions[i], 0, false);
}
