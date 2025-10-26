# Guide

## Conventions to be familiar with

- `!` usage::
    - Indicates a namespace or member variable of a C++ type, e.g., `PolygonMode!FILL` refers to the `FILL` enum inside `PolygonMode`.
    - Marks a mutable function (setter), for example, `x!(myvec::Vec3)` sets the x value, while `x(myvec::Vec3)` is the getter.
- Using the `Lys` module:
    - Due to C++ wrapping limitations, most methods and enums require the `Lys.` prefix when called from Julia. 
    - Struct types like `Node3D` and some special types like `Vec3` allow using their functions (e.g., `x()`) without the `Lys.` prefix.
    - If you wish to avoid using the prefix `Lys.` for certain methods, types, etc., replace `using Lys` with `import Lys: _init, etc.`