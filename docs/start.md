Getting Started
===============

```
$ git clone https://github.com/LarocCC/laroc.git

$ cd laroc

$ ls
docs  include  LICENSE  Makefile  README.md  src  test  vendor

$ ls vendor/stb/stb_ds.h
ls: cannot access 'vendor/stb/stb_ds.h': No such file or directory

$ git submodule update --init vendor/stb

$ ls vendor/stb/stb_ds.h
vendor/stb/stb_ds.h

$ make

$ ./build/bin/laroc
Usage: ./build/bin/laroc input.c -S -o output.s

$ make test
```
