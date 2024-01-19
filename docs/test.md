Testing
=======

**In-tree tests** can be placed anywhere under `src/`. They are named as
  `*.test.c`. Each of them are compiled to a single executable. The executables
  are expected to return 0.

**Test inputs** for `laroc` are placed under `test/`. Valid inputs are
  `test/<pass_name>/*.c` while invalid inputs are `test/<pass_name>/*.xfail.c`,
  where `*` is an arbitary string.

- **For valid inputs**, they should have a corresponding
  `test/<pass_name>/*.out.txt`. `laorc` is expected to return 0 when running
  `laroc test/<pass_name>/*.c -print-after=<pass_name>`, and its output will be
  compared to `test/<pass_name>/*.out.txt`.

- **For invalid inputs**, `laorc` is expected to return a non-zero value when
  running `laroc test/<pass_name>/*.xfail.c -print-after=<pass_name>`.

Both **in-tree tests** and **test inputs** will be checked when invoking `make
test`.
