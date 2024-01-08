#ifndef LAROC_SEMA_TRANSUNIT_H
#define LAROC_SEMA_TRANSUNIT_H

#include "typedef.h"

struct TranslationUnit {
  SymTable *symtab;
  Declaration **decltions;
};

TranslationUnit *newTranslationUnit();

void printTranslationUnit(TranslationUnit *unit);

#endif
