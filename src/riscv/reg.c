#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "riscv/reg.h"

void printReg(FILE *fp, Reg r) {
  if (r >= FRAME_OBJ_ID_BEGIN) {
    fprintf(fp, "{%%F%d}", r - FRAME_OBJ_ID_BEGIN);
    return;
  }

  if (r >= VIRT_REG_BEGIN) {
    fprintf(fp, "%%%d", r - VIRT_REG_BEGIN);
    return;
  }

  switch (r) {
  case RV_ZERO:
    fprintf(fp, "zero");
    return;
  case RV_RA:
    fprintf(fp, "ra");
    return;
  case RV_SP:
    fprintf(fp, "sp");
    return;
  case RV_GP:
    fprintf(fp, "gp");
    return;
  case RV_TP:
    fprintf(fp, "tp");
    return;
  case RV_T0:
    fprintf(fp, "t0");
    return;
  case RV_T1:
    fprintf(fp, "t1");
    return;
  case RV_T2:
    fprintf(fp, "t2");
    return;
  case RV_S0:
    fprintf(fp, "s0");
    return;
  case RV_S1:
    fprintf(fp, "s1");
    return;
  case RV_A0:
    fprintf(fp, "a0");
    return;
  case RV_A1:
    fprintf(fp, "a1");
    return;
  case RV_A2:
    fprintf(fp, "a2");
    return;
  case RV_A3:
    fprintf(fp, "a3");
    return;
  case RV_A4:
    fprintf(fp, "a4");
    return;
  case RV_A5:
    fprintf(fp, "a5");
    return;
  case RV_A6:
    fprintf(fp, "a6");
    return;
  case RV_A7:
    fprintf(fp, "a7");
    return;
  case RV_S2:
    fprintf(fp, "s2");
    return;
  case RV_S3:
    fprintf(fp, "s3");
    return;
  case RV_S4:
    fprintf(fp, "s4");
    return;
  case RV_S5:
    fprintf(fp, "s5");
    return;
  case RV_S6:
    fprintf(fp, "s6");
    return;
  case RV_S7:
    fprintf(fp, "s7");
    return;
  case RV_S8:
    fprintf(fp, "s8");
    return;
  case RV_S9:
    fprintf(fp, "s9");
    return;
  case RV_S10:
    fprintf(fp, "s10");
    return;
  case RV_S11:
    fprintf(fp, "s11");
    return;
  case RV_T3:
    fprintf(fp, "t3");
    return;
  case RV_T4:
    fprintf(fp, "t4");
    return;
  case RV_T5:
    fprintf(fp, "t5");
    return;
  case RV_T6:
    fprintf(fp, "t6");
    return;
  default:
    assert(false);
  }
}

bool regIsPhysical(Reg r) { return r >= 0 && r <= 31; }

bool regIsVirtual(Reg r) {
  return r >= VIRT_REG_BEGIN && r < FRAME_OBJ_ID_BEGIN;
}

bool regIsFrameObject(Reg r) {
  return r >= FRAME_OBJ_ID_BEGIN && r < REG_INVAL;
}

bool regArrIsSame(Reg *arr1, Reg *arr2) {
  if (arrlen(arr1) != arrlen(arr2))
    return false;
  for (int i = 0; i < arrlen(arr1); i++) {
    if (arr1[i] != arr2[i])
      return false;
  }
  return true;
}

bool regArrIncludeReg(Reg *arr, Reg r) {
  for (int i = 0; i < arrlen(arr); i++) {
    if (arr[i] == r)
      return true;
  }
  return false;
}

void regArrRemoveReg(Reg *arr, Reg r) {
  for (int i = 0; i < arrlen(arr); i++) {
    if (arr[i] == r)
      arr[i] = arrlast(arr);
  }
  arrpop(arr);
}

static int compareReg(const void *r1, const void *r2) {
  return *(const Reg *)r1 - *(const Reg *)r2;
}

void makeRegSet(Reg *regs) {
  qsort(regs, arrlen(regs), sizeof(Reg), compareReg);

  Reg *end = regs + arrlen(regs);
  Reg *p = regs, *q = regs;
  while (q < end) {
    *p = *q;
    do
      q++;
    while (q < end && *p == *q);
    p++;
  }
  arrsetlen(regs, p - regs);
}

Reg *mergeRegSet(Reg *arr1, Reg *arr2) {
  Reg *res = NULL;
  int i = 0, j = 0;
  while (i < arrlen(arr1) && j < arrlen(arr2)) {
    if (arr1[i] == arr2[j]) {
      j++;
      continue;
    }
    if (arr1[i] < arr2[j]) {
      arrput(res, arr1[i]);
      i++;
    } else {
      arrput(res, arr2[j]);
      j++;
    }
  }
  for (; i < arrlen(arr1); i++)
    arrput(res, arr1[i]);
  for (; j < arrlen(arr2); j++)
    arrput(res, arr2[j]);
  return res;
}

Reg *subtractRegSet(Reg *arr1, Reg *arr2) {
  Reg *res = NULL;
  int i = 0, j = 0;
  while (i < arrlen(arr1) && j < arrlen(arr2)) {
    if (arr1[i] == arr2[j]) {
      i++;
      j++;
      continue;
    }
    if (arr1[i] < arr2[j]) {
      arrput(res, arr1[i]);
      i++;
    } else {
      j++;
    }
  }
  for (; i < arrlen(arr1); i++)
    arrput(res, arr1[i]);
  return res;
}
