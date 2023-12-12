#include <assert.h>
#include <string.h>

#include "stb_ds.h"

#include "lex/kwd.h"
#include "lex/lex.h"
#include "lex/punct.h"
#include "lex/token.h"

int main() {
  const char *source = "int main() { return 0; }";
  //                    0   1   23 4 5      67 89

  Token *tokens = lex(source, strlen(source));

  assert(arrlen(tokens) == 10);

  assert(tokenIsKwd(&tokens[0], KWD_INT));
  assert(tokens[1].kind == TOK_IDENT);
  assert(strcmp(tokens[1].ident, "main") == 0);
  assert(tokenIsPunct(&tokens[2], PUNCT_PAREN_L));
  assert(tokenIsPunct(&tokens[3], PUNCT_PAREN_R));
  assert(tokenIsPunct(&tokens[4], PUNCT_BRACE_L));
  assert(tokenIsKwd(&tokens[5], KWD_RETURN));
  assert(tokens[6].kind == TOK_NUM);
  assert(tokens[6].num == 0);
  assert(tokenIsPunct(&tokens[7], PUNCT_SEMICOLON));
  assert(tokenIsPunct(&tokens[8], PUNCT_BRACE_R));
  assert(tokens[9].kind == TOK_EOF);

  return 0;
}
