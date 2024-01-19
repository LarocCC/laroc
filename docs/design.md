Design Principles
=================

- Use C99, avoid GCC's extensions.

- Use [stb_ds](http://nothings.org/stb_ds/) (`#include "stb/stb_ds.h"`) for
  dynamic arrays and hash tables.

- All `typedef`s of structs and enums are placed in `include/typedef.h`. This is
  for preventing header files recursivly including each other only for using
  pointers to other structs.

- Zero values are considered an initial state of all structs. It should
  represent either an invalid state (e.g. `EXPR_INVAL` in
  `include/parse/expr.h`), or a no-op state (e.g. `STMT_EMPTY` in
  `include/parse/stmt.h`). Thus, all structs are zeroed before being passed to
  functions as result values. Also, for the same reason, we allocate heap memory
  with `calloc` instead of `malloc` to simplify code.

- Lex functions scan start at `begin` and stop at `end` or an unrecognized
  character. They store to `result` and return the number of characters
  consumed.

  ```c
  int scanX(const char *begin, const char *end, X *result);
  ```

- Parse functions are like lex functions, except they stop at a `TOK_EOF` or an
  unrecognized token.

  ```c
  int parseX(ParseCtx *ctx, const Token *begin, X *result);
  ```
