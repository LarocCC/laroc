#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/parse.h"
#include "sema/decl.h"
#include "sema/symbol.h"
#include "sema/transunit.h"
#include "util/diag.h"

TranslationUnit *parseTranslationUnit(const Token *tokens) {
  TranslationUnit *unit = newTranslationUnit();

  ParseCtx ctx;
  memset(&ctx, 0, sizeof(ParseCtx));
  ctx.symtab = unit->symtab;

  const Token *p = tokens;
  const Token *eofToken = &tokens[arrlen(tokens) - 1];
  int n;

  while (p < eofToken) {
    Declaration *decltion = calloc(1, sizeof(Declaration));
    if ((n = parseDeclaration(&ctx, p, decltion)) == 0) {
      free(decltion);
      emitDiagnostic(p->loc, "Unexpected token");
    }
    p += n;
    arrput(unit->decltions, decltion);
  }

  return unit;
}
