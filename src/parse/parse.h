#ifndef LAROC_PARSE_PARSE_H
#define LAROC_PARSE_PARSE_H

#include "typedef.h"

struct ParseCtx {
  Declaration *func;
  SymTable *symtab;
};

struct TranslationUnit {
  SymTable *symtab;
  Declaration **decltions;
};

TranslationUnit *parseTranslationUnit(const Token *tokens);

void printTranslationUnit(TranslationUnit *unit);

#endif
