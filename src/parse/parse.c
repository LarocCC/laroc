#include <stdio.h>
#include <stdlib.h>

#include "stb_ds.h"

#include "typedef.h"
#include "lex/token.h"
#include "parse/decl.h"
#include "parse/parse.h"
#include "parse/symbol.h"

TranslationUnit *parseTranslationUnit(const Token *tokens) {
  TranslationUnit *tu = calloc(1, sizeof(TranslationUnit));
  tu->symtab = newSymTable(NULL);

  ParseCtx *ctx = calloc(1, sizeof(ParseCtx));
  ctx->symtab = tu->symtab;

  const Token *p = tokens;
  const Token *eofToken = &tokens[arrlen(tokens) - 1];
  int n;

  while (p < eofToken) {
    Declaration *decltion = calloc(1, sizeof(Declaration));
    if ((n = parseDeclaration(ctx, p, decltion)) == 0) {
      free(decltion);
      printf("unexpected token ");
      printToken(p);
      exit(1);
    }
    p += n;
    arrput(tu->decltions, decltion);
  }

  return tu;
}
