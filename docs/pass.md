Compiler Passes
===============

This page lists all compiler passes in order. The checkboxs in the beginning of
each line indicate whether it is currently implemeted. The code-marked string
after is the internal pass name. To view the result after the pass, pass
`-print-after=<pass_name>` to `laroc`.

Lex
---

- [x] `lex`: Transform input from an array of characters (`const char *`) to an
array of tokens (`const Token *`).

Parse
-----

- [x] `parse`: Transform input from an array of tokens (`const Token *`) to an AST
(`TranslationUnit *`). This pass also checks the type of expesssions.

Semantic Passes
---------------

These passes validate, analysis and transform the AST.

- [x] `labelchk`: Check identifiers after `goto` are valid labels.

IR Genreation
-------------

- [x] `irgen`: Transform the AST (`TranslationUnit *`) to an IR module (`Module *`).

IR Passes
---------

These passes analysis and transform the IR module.

- [ ] `ptranalysis`

- [ ] `unneedload`

- [ ] `deadstore`

- [x] `dag`: Transform the IR module from a list of instructions to a list of
  DAG.

Instruction Selection
---------------------

- [x] `isel`: Transform the IR module (`Module *`) to a RISC-V object file
(`ObjectFile *`).

RISC-V Passes
-------------

These passes analysis and transform the RISC-V object file.

- [x] `liveanalysis`: Analysis the live range of registers and frame objects.

- [x] `regalloc`: Allocate physical registers to virtual registers.

- [x] `pei`: Prologue and epilogue insertion for functions. Replace frame object
  operands with offset to `sp`.
