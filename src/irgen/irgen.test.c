#include <assert.h>
#include <string.h>

#include "stb_ds.h"

#include "ir/ir.h"
#include "irgen/func.h"
#include "irgen/irgen.h"
#include "lex/lex.h"
#include "parse/parse.h"

int main() {
  const char *source;
  Module *mod;

  source = "int main(int argc) {}";
  mod = genIR(parseTranslationUnit(lex(source, strlen(source))));
  assert(arrlen(mod->funcs) == 1);
  assert(strcmp(mod->funcs[0]->name, "main") == 0);
  assert(mod->funcs[0]->ret->kind == IR_I32);
  assert(arrlen(mod->funcs[0]->args) == 1);
  assert(mod->funcs[0]->args[0]->kind == IR_VAL_VAR);
  assert(strcmp(mod->funcs[0]->args[0]->name, "argc") == 0);
  assert(mod->funcs[0]->args[0]->ty->kind == IR_I32);

  return 0;
}
