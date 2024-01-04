#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "typedef.h"
#include "ir/func.h"
#include "riscv/block.h"
#include "riscv/frameobj.h"
#include "riscv/func.h"

RVFunc *newRVFunc(IRFunc *irFunc) {
  RVFunc *rvFunc = calloc(1, sizeof(RVFunc));
  rvFunc->name = irFunc->name;

  for (int i = 0; i < arrlen(irFunc->allocas); i++)
    funcAddFrameObjectFromAlloca(rvFunc, irFunc->allocas[i]);

  rvFunc->blockCount = irFunc->blockCount;
  arrput(rvFunc->blocks, NULL);

  return rvFunc;
}

void printRVFunc(RVFunc *func) {
  printf("%s:\n", func->name);

  printf("# entry = .B%d\n", func->entry->id);
  printf("# exits =");
  for (int i = 0; i < arrlen(func->exits); i++)
    printf(" .B%d", func->exits[i]->id);
  printf("\n");

  for (int i = 0; i < arrlen(func->frameObjs); i++)
    printFrameObject(func->frameObjs[i]);

  for (int i = 1; i <= func->blockCount; i++)
    printRVBlock(func->blocks[i]);
}
