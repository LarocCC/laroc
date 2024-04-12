  Progress
========

This page lists the current support to all compiler and C features.

| Icon | Meaning        |
|------|----------------|
| ❌   | No             |
| 🔵   | Partial        |
| ✅   | Yes            |
| -    | Not applicable |

Compiler
--------

|    | Feature | Note |
|----|---------|------|
| ❌ | multiple inputs | |
| ✅ | `-o` | |
| ❌ | `-E` | |
| ✅ | `-S` | |
| ✅ | `-c` | |
| ❌ | with none of `-E`, `-S`, or `-c` | |

Lex
---

|    | Feature | Note |
|----|---------|------|
| ❌ | preprocessing tokens | |
| ✅ | keyword, identifier | |
| 🔵 | integer | `0x`, `0o`, suffixes |
| ❌ | float | |
| ❌ | char and string literals | |
| ✅ | punctuator | |

Preprocessing
-------------

❌ Currently laroc has no preprocessor at all.

Declaration
-----------

| Parse | Sema | IR Gen | Feature | Note |
|-------|------|--------|---------|------|
| ✅    | ✅   | ✅     | integers | |
| ✅    | ✅   | ✅     | `_Bool` | |
| ❌    | ❌   | ❌     | `float`, `double` | |
| ❌    | ❌   | ❌     | `struct`, `union` | |
| ❌    | ❌   | ❌     | `enum` | |
| ❌    | ❌   | ❌     | `typedef` | |
| ❌    | ❌   | ❌     | `const`, `restrict`, `volatile` | |
| ❌    | ❌   | ❌     | `extern`, `static`, `auto`, `register` | |
| ❌    | ❌   | ❌     | `inline` | |
| ❌    | ❌   | ❌     | pointer | |
| 🔵    | 🔵   | 🔵     | function declarator | variadic, `, ...` |
| 🔵    | 🔵   | 🔵     | initializer | initializer list, `= { ... }` |
| ✅    | ✅   | ✅     | function defination | |

Expression
----------

| Parse | Sema | IR Gen | Category | Feature | Note |
|-------|------|--------|----------|---------|------|
| -     | ✅   | 🔵     | Primary  | integer constant | 64-bit integers |
| -     | ❌   | ❌     | Primary  | float constant | |
| -     | ❌   | ❌     | Primary  | char and string literals | |
| ✅    | ✅   | ✅     | Primary  | identifier | |
| ✅    | -    | -      | Primary  | `(x)` | |
| ❌    | ❌   | ❌     | Postfix  | `a[i]` | |
| ❌    | ❌   | ❌     | Postfix  | `f(a, b)` | |
| 🔵    | ✅   | ❌     | Postfix  | `s.x` | |
| 🔵    | ✅   | ❌     | Postfix  | `s->x` | |
| 🔵    | ✅   | ❌     | Postfix  | `x++`, `x--` | pointer |
| ❌    | ❌   | ❌     | Postfix  | `(T){ ... }` | |
| 🔵    | ❌   | ❌     | Unary    | `++x`, `--x` | pointer |
| ❌    | ✅   | ❌     | Unary    | `&x`, `*x` | |
| ✅    | ✅   | ❌     | Unary    | `+x`, `-x` | |
| ❌    | ✅   | ❌     | Unary    | `~x` | |
| ❌    | ✅   | ❌     | Unary    | `!x` | |
| ❌    | ✅   | ❌     | Unary    | `sizeof x` | |
| ❌    | ✅   | ❌     | Unary    | `sizeof(T)` | |
| ❌    | ❌   | ❌     | Cast     | `(T)x` | |
| 🔵    | ✅   | ❌     | Mul      | `x*y` | cast operands |
| ❌    | ✅   | ❌     | Mul      | `x/y` | |
| ❌    | ✅   | ❌     | Mul      | `x%y` | |
| 🔵    | ✅   | 🔵     | Additive | `x+y` | cast operands, pointer |
| 🔵    | ✅   | 🔵     | Additive | `x-y` | cast operands, pointer |
| ❌    | ✅   | ❌     | Shift    | `x<<y`, `x>>y` | |
| ❌    | ✅   | ❌     | Relation | `x<y`, `x>y`, `x<=y`, `x>=y` | |
| ❌    | ✅   | ❌     | Equality | `x==y`, `x!=y` | |
| ❌    | ✅   | ❌     | Bit      | `x&y`, `x^y`, `x\|y` | |
| ❌    | ✅   | ❌     | Logic    | `x&&y`, `x\|\|y` | |
| 🔵    | ✅   | 🔵     | Cond     | `x?y:z` | only support arithmetic types, cast operands |
| 🔵    | ✅   | 🔵     | Assign   | `x=y`  | only support arithmetic types |
| ❌    | ✅   | ❌     | Assign   | `x*=y`, `x/=y`, `x%=y` | |
| ❌    | ✅   | ❌     | Assign   | `x+=y`, `x-=y` | |
| ❌    | ✅   | ❌     | Assign   | `x<<=y`, `x>>=y` | |
| ❌    | ✅   | ❌     | Assign   | `x&=y`, `x^=y`, `x\|=y` | |
| 🔵    | ✅   | ❌     | Comma    | `x,y` | shouldn't yield lvalue |

Statement
----------

| Parse | Sema | IR Gen | Category   | Feature | Note |
|-------|------|--------|------------|---------|------|
| ✅    | ✅   | ✅     | Label      | `label:` | |
| ✅    | ✅   | ✅     | Compound   | `{ ... }` | |
| ✅    | ✅   | ✅     | Expression | `expr;` | |
| ✅    | ✅   | ✅     | Null       | `;` | |
| ✅    | ✅   | ✅     | Selection  | `if` | |
| ❌    | ❌   | ❌     | Selection  | `switch` | |
| ❌    | ❌   | ❌     | Iteration  | `while` | |
| ❌    | ❌   | ❌     | Iteration  | `do ... while` | |
| ❌    | ❌   | ❌     | Iteration  | `for` | |
| ✅    | ✅   | ✅     | Jump       | `goto` | |
| ❌    | ❌   | ❌     | Jump       | `continue` | |
| ❌    | ❌   | ❌     | Jump       | `break` | |
| 🔵    | ✅   | ✅     | Jump       | `return` | cast operand |
