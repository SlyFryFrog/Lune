# Lys

## What is Lys?

Lys is a game engine that is written in modern C++23 using modules. The engine currently supports the following features:

- OpenGL backend (*Incomplete)
    - Wraps method calls and actions to simplify interacting directly with OpenGL
    - Supports loading and rendering meshes for `.obj`
    - Supports hot-reloading `.glsl` shaders
    - Missing some features that require interacting with `<GL/glew.h>`
- Optional Scripting support (*Incomplete)
    - Julia scripting with hot-reloading available.
- No default render loop
    - Currently, acts more like a "framework", although this will change
- Multi-platform
    - Supports both macOS and Linux (tested on Ubuntu25.04+)
    - No Windows support is planned

## Questions

#### Why the name Lys?

Due to my love of French language and culture, I wanted to choose a name that reflected this. La fleur de lys is an important symbol of French heritage and identity, which made it the perfect choice.

#### Why Julia?

Julia was chosen as the scripting language due to its fast performance (although it suffers from JIT lag). It also has little documentation on how to use libraries such as CxxWrap, so I wanted to challenge myself and potentially add value to its integration by providing a real-world example.
