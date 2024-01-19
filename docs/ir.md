Intermediate Representation
===========================

Comments
--------

`; comment`

Types
-----

- `void`

- `ptr`

- `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`

- `f32`, `f64`

TODO: Add aggregated types.

Values
------

Void: `void`

Immediate: `0`, `123`

Variable: `ptr %1`, `i32 %2`

Block: `.B1`

Instructions
------------

TODO: Add function call.

**Alloca**

```
ptr %1 = alloca 4, 4
```

**Load and store**

```
i32 %2 = load ptr %1
```

```
store ptr %1, i32 %3
```

```
i32 %1 = li 123
```

**Cast**

```
i32 %2 = cast i32 %1
```

**Arithmetic operations**

```
i32 %3 = add i32 %1, i32 %2
i32 %4 = sub i32 %1, i32 %2
i32 %5 = mul i32 %1, i32 %2
i32 %6 = div i32 %1, i32 %2
i32 %7 = mod i32 %1, i32 %2
i32 %8 = not i32 %1
i32 %9 = shl i32 %1, i32 %2
i32 %10 = shr i32 %1, i32 %2
i32 %11 = and i32 %1, i32 %2
i32 %12 = xor i32 %1, i32 %2
i32 %13 = or i32 %1, i32 %2
```

**Comparations**

```
i32 %3 = cmp.eq i32 %1, i32 %2
i32 %4 = cmp.lt i32 %1, i32 %2
i32 %5 = cmp.le i32 %1, i32 %2
```

The result value is set to `1` if the operands match the condition, or the result
will be set to `0` otherwise.

**Unconditional Branch**

```
j .blk
```

**Conditional Branch**

```
br %1, .blkT, .blkF
```

The program's control flow will jump to `.blkT` is `%1` is a non-zero value, or
will jump to `.blkF` otherwise.

**Phi**

```
%3 = phi .B1, %1, .B2, %2, ...
```

**Return**

```
ret void
ret i32 %1
```

Basic Block
-----------

A basic block has any number of predecessor and successor blocks.

```
.B1:
; preds =
; succs =
  store ptr %2, i32 %1
  i32 %3 = load ptr %2
  i32 %4 = li 1
  i32 %5 = sub i32 %3, i32 %4
  ret i32 %5
```

Function
--------

A function contains exactly one entry block and any number of exit blocks.

Static on-stack allocations are stored in the function, but not in any blocks.

```
func i32 @main(i32 %1) {
; entry = .B1
; exits = .B1
  ptr %2 = alloca 4, 4

.B1:
; preds =
; succs =
  store ptr %2, i32 %1
  i32 %3 = load ptr %2
  i32 %4 = li 1
  i32 %5 = sub i32 %3, i32 %4
  ret i32 %5
}
```
