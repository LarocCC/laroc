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

Immediate: `0`, `123`

Variable: `ptr %1`, `i32 %2`

Instructions
------------

```
ptr %1 = alloca 4, 4
```

```
i32 %2 = load ptr %1
store ptr %1, i32 %3
```

```
i32 %3 = add i32 %1, i32 %2
i32 %4 = sub i32 %1, i32 %2
```

```
ret void
ret i32 %1
```
