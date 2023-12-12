#include <assert.h>
#include <string.h>

#include "lex/kwd.h"
#include "lex/punct.h"
#include "lex/token.h"

int main() {
  Token tok;

  assert(scanToken("int", &tok) == 3);
  assert(tok.kwd == KWD_INT);

  assert(scanToken("laroc", &tok) == 5);
  assert(strcmp(tok.ident, "laroc") == 0);

  assert(scanToken("123", &tok) == 3);
  assert(tok.num == 123);

  assert(scanToken(";", &tok) == 1);
  assert(tok.punct == PUNCT_SEMICOLON);

  return 0;
}
