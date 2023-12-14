#include <assert.h>
#include <string.h>

#include "stb_ds.h"

#include "lex/lex.h"
#include "lex/token.h"
#include "parse/type.h"

int main() {
  const char *source;
  Token *tokens;
  CType ty;

  source = "laroc";
  tokens = lex(source, strlen(source));
  assert(parseSpecifier(tokens, &ty) == 0);

  source = "int";
  tokens = lex(source, strlen(source));
  assert(parseSpecifier(tokens, &ty) == 1);
  assert(ty.kind == TYPE_INT);

  return 0;
}
