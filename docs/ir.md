Intermediate Representation
===========================

Types
-----

- `void`

- `ptr`

- `i8`, `i16`, `i32`, `i64`, `u8`, `u16`, `u32`, `u64`

- `f32`, `f64`

Values
------

Void: `void`

Immediate: `0`, `123`

Variable: `ptr %1`, `i32 %2`

Block: `.B1`

Instructions
------------

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
i32 %14 = cmp.eq i32 %1, i32 %2
i32 %15 = cmp.lt i32 %1, i32 %2
i32 %16 = cmp.le i32 %1, i32 %2
```

**Branches**

```
j .blk
```

```
br %1, .blkT, .blkF
```

**Phi**

```
%3 = phi .B1, %1, .B2, %2, ...
```

**Return**

```
ret void
ret i32 %1
```
