Testing
=======

**In-tree tests** can be placed anywhere under `src/`. They are named as
  `*.test.c`. Each of them are compiled to a single executable.

- Test inputs for `laroc` are placed under `test/`. Valid inputs are named as
  `*.c` while invalid inputs are named as `*.xfail.c`, where `*` is an arbitary
  string.

- Both in-tree tests and test inputs for `laroc` are checked by `make test`.
