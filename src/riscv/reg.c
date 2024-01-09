#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "stb/stb_ds.h"

#include "riscv/reg.h"

void printReg(Reg r) {
  if (r >= FRAME_OBJ_ID_BEGIN) {
    printf("{%%F%d}", r - FRAME_OBJ_ID_BEGIN);
    return;
  }

  if (r >= VIRT_REG_BEGIN) {
    printf("%%%d", r - VIRT_REG_BEGIN);
    return;
  }

  switch (r) {
  case RV_ZERO:
    printf("zero");
    return;
  case RV_RA:
    printf("ra");
    return;
  case RV_SP:
    printf("sp");
    return;
  case RV_GP:
    printf("gp");
    return;
  case RV_TP:
    printf("tp");
    return;
  case RV_T0:
    printf("t0");
    return;
  case RV_T1:
    printf("t1");
    return;
  case RV_T2:
    printf("t2");
    return;
  case RV_S0:
    printf("s0");
    return;
  case RV_S1:
    printf("s1");
    return;
  case RV_A0:
    printf("a0");
    return;
  case RV_A1:
    printf("a1");
    return;
  case RV_A2:
    printf("a2");
    return;
  case RV_A3:
    printf("a3");
    return;
  case RV_A4:
    printf("a4");
    return;
  case RV_A5:
    printf("a5");
    return;
  case RV_A6:
    printf("a6");
    return;
  case RV_A7:
    printf("a7");
    return;
  case RV_S2:
    printf("s2");
    return;
  case RV_S3:
    printf("s3");
    return;
  case RV_S4:
    printf("s4");
    return;
  case RV_S5:
    printf("s5");
    return;
  case RV_S6:
    printf("s6");
    return;
  case RV_S7:
    printf("s7");
    return;
  case RV_S8:
    printf("s8");
    return;
  case RV_S9:
    printf("s9");
    return;
  case RV_S10:
    printf("s10");
    return;
  case RV_S11:
    printf("s11");
    return;
  case RV_T3:
    printf("t3");
    return;
  case RV_T4:
    printf("t4");
    return;
  case RV_T5:
    printf("t5");
    return;
  case RV_T6:
    printf("t6");
    return;
  default:
    assert(false);
  }
}

bool regIsPhysical(Reg r) { return r >= 0 && r <= 31; }

bool regIsVirtual(Reg r) {
  return r >= VIRT_REG_BEGIN && r < FRAME_OBJ_ID_BEGIN;
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
    while (*p == *q);
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
