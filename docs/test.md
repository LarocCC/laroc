Testing
=======

The development of laroc is almost test-driven
([TDD](https://en.wikipedia.org/wiki/Test-driven_development)), so please always
update or add test files if the commit contains functional change.

Laroc uses two types of test: **in-tree tests** and **test inputs**. Both of
them will be checked when invoking `make test`.

In-tree tests
-------------

In-tree tests can be placed anywhere under `src/`. They are named as `*.test.c`.
Each of them are compiled to a single executable. The executables are expected
to return 0. See `src/sema/symbol.test.c` for an example.

Test inputs
-----------
Test inputs for laroc are placed under `test/`. Valid inputs are
`test/<pass_name>/*.c` while invalid inputs are `test/<pass_name>/*.xfail.c`,
where `*` is an arbitary string.

- **For valid inputs**, they should have a corresponding
  `test/<pass_name>/*.out.txt`. `laorc` is expected to return 0 when running
  `laroc test/<pass_name>/*.c -print-after=<pass_name>`, and its output will be
  compared to `test/<pass_name>/*.out.txt`.

  During `make test`, the actual outputs of laorc (not the expected outputs) are
  written to `build/test/<pass_name>/*.out.txt`. You may copy all
  `build/test/**/*.out.txt` to `test/**/*.out.txt` by `make
  update-test-outputs`. Remember to double check the changes before commit to
  avoid including irrelevant changes.

- **For invalid inputs**, laorc is expected to return a non-zero value when
  running `laroc test/<pass_name>/*.xfail.c -print-after=<pass_name>`.
