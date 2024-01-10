#ifndef LAROC_SEMA_TRANSUNIT_H
#define LAROC_SEMA_TRANSUNIT_H

#include "typedef.h"

/// A translation unit.
struct TranslationUnit {
  /// Symbol table contains all top-level declarations and function definations.
  SymTable *symtab;

  /// All top-level declarations and function definations.
  Declaration **decltions;
};

TranslationUnit *newTranslationUnit();

void printTranslationUnit(TranslationUnit *unit);

#endif
