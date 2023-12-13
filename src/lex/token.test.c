#include <assert.h>
#include <string.h>

#include "lex/kwd.h"
#include "lex/number.h"
#include "lex/punct.h"
#include "lex/token.h"

int main() {
  const char *begin, *end;
  Token tok;

  begin = "int";
  end = begin + strlen(begin);
  assert(scanToken(begin, end, &tok) == 3);
  assert(tokenIsKwd(&tok, KWD_INT));

  begin = "laroc";
  end = begin + strlen(begin);
  assert(scanToken(begin, end, &tok) == 5);
  assert(tok.kind == TOK_IDENT);
  assert(strcmp(tok.ident, "laroc") == 0);

  begin = "123";
  end = begin + strlen(begin);
  assert(scanToken(begin, end, &tok) == 3);
  assert(tok.kind == TOK_NUM);
  assert(tok.num->x == 123);

  begin = ";";
  end = begin + strlen(begin);
  assert(scanToken(begin, end, &tok) == 1);
  assert(tokenIsPunct(&tok, PUNCT_SEMICOLON));

  return 0;
}
