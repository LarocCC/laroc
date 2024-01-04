#include <stdbool.h>
#include <stdio.h>

#include "stb/stb_ds.h"

#include "ir/block.h"
#include "ir/func.h"
#include "ir/inst.h"
#include "ir/type.h"
#include "ir/value.h"

IRFunc *newIRFunc(const char *name) {
  IRFunc *func = calloc(1, sizeof(IRFunc));
  func->name = name;
  arrput(func->blocks, NULL);
  return func;
}

void printIRFunc(IRFunc *func) {
  printf("func ");
  printIRType(func->ret);
  printf(" @%s(", func->name);
  for (int i = 0; i < arrlen(func->args); i++) {
    if (i != 0)
      printf(", ");
    printValue(func->args[i]);
  }
  printf(") {\n");

  printf("; entry = .B%d\n", func->entry->id);
  printf("; exits =");
  for (int i = 0; i < arrlen(func->exits); i++)
    printf(" .B%d", func->exits[i]->id);
  printf("\n");

  for (int i = 0; i < arrlen(func->allocas); i++)
    printIRInst(func->allocas[i], true);

  for (int i = 1; i <= func->blockCount; i++)
    printIRBlock(func->blocks[i]);

  printf("}\n");
}
