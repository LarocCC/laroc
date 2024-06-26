#ifndef LAROC_RISCV_REG_H
#define LAROC_RISCV_REG_H

#include <stdbool.h>
#include <stdio.h>

#include "typedef.h"

/// A RISC-V register.
///
/// The value matches DWARF register numbers. Virtual registers use the range
/// [8192, 16384). Frame objects use the range [16384, 32768). An invalid
/// register is 32768.
typedef enum Reg {
  RV_X0 = 0,
  RV_X1 = 1,
  RV_X2 = 2,
  RV_X3 = 3,
  RV_X4 = 4,
  RV_X5 = 5,
  RV_X6 = 6,
  RV_X7 = 7,
  RV_X8 = 8,
  RV_X9 = 9,
  RV_X10 = 10,
  RV_X11 = 11,
  RV_X12 = 12,
  RV_X13 = 13,
  RV_X14 = 14,
  RV_X15 = 15,
  RV_X16 = 16,
  RV_X17 = 17,
  RV_X18 = 18,
  RV_X19 = 19,
  RV_X20 = 20,
  RV_X21 = 21,
  RV_X22 = 22,
  RV_X23 = 23,
  RV_X24 = 24,
  RV_X25 = 25,
  RV_X26 = 26,
  RV_X27 = 27,
  RV_X28 = 28,
  RV_X29 = 29,
  RV_X30 = 30,
  RV_X31 = 31,

  RV_ZERO = 0,
  RV_RA = 1,
  RV_SP = 2,
  RV_GP = 3,
  RV_TP = 4,
  RV_T0 = 5,
  RV_T1 = 6,
  RV_T2 = 7,
  RV_S0 = 8,
  RV_FP = 8,
  RV_S1 = 9,
  RV_A0 = 10,
  RV_A1 = 11,
  RV_A2 = 12,
  RV_A3 = 13,
  RV_A4 = 14,
  RV_A5 = 15,
  RV_A6 = 16,
  RV_A7 = 17,
  RV_S2 = 18,
  RV_S3 = 19,
  RV_S4 = 20,
  RV_S5 = 21,
  RV_S6 = 22,
  RV_S7 = 23,
  RV_S8 = 24,
  RV_S9 = 25,
  RV_S10 = 26,
  RV_S11 = 27,
  RV_T3 = 28,
  RV_T4 = 29,
  RV_T5 = 30,
  RV_T6 = 31,

  VIRT_REG_BEGIN = 8192,
  FRAME_OBJ_ID_BEGIN = 16384,
  REG_INVAL = 32768,
} Reg;

void printReg(FILE *fp, Reg r);

bool regIsPhysical(Reg r);
bool regIsVirtual(Reg r);
bool regIsFrameObject(Reg r);

bool regArrIsSame(Reg *arr1, Reg *arr2);
bool regArrIncludeReg(Reg *arr, Reg r);
void regArrRemoveReg(Reg *arr, Reg r);
void makeRegSet(Reg *regs);
Reg *mergeRegSet(Reg *arr1, Reg *arr2);
Reg *subtractRegSet(Reg *arr1, Reg *arr2);

#endif
