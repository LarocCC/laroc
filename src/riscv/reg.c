#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "riscv/reg.h"

void printReg(Reg r) {
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
