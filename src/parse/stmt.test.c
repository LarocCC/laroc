#include <assert.h>
#include <string.h>

#include "lex/lex.h"
#include "lex/token.h"
#include "parse/stmt.h"

int main() {
  const char *source;
  Token *tokens;
  Stmt stmt;

  source = "{}";
  tokens = lex(source, strlen(source));
  assert(parseCmpdStmt(tokens, &stmt) == 2);
  assert(stmt.kind == STMT_CMPD);

  return 0;
}
