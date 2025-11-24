# Lune

Lune is a work-in-progress game framework for macOS and Linux. It uses Metal on macOS and Vulkan on both macOS and
Linux. The library is written in modern C++ and encourages RAII for safer and
cleaner code.

**Note: XCode can't be used as it currently doesn't support `.cppm` modules, unlike ninja**

## Feature Goals

- [x] Platform independent input and window management system (w/ GLFW).
- [x] Metal Shading Language (MSL) for macOS.
- [ ] Support for Slang (for cross-platform shaders).
- [ ] Support for compute and graphic-related shaders:
  - [x] Metal compute shaders.
  - [x] Metal vertex and fragment shaders.
  - [ ] Vulkan compute shaders.
  - [ ] Vulkan vertex and fragment shaders.
- [ ] Load and render 3D meshes (e.g., OBJ, glTF).
- [ ] A simple rendering API that works the same way on both Metal and Vulkan.
- [ ] Shader hot-reloading for faster development.
- [ ] Basic debugging tools (e.g., wireframe mode, GPU profiling).
- [ ] ImGui support for in-engine UI and debugging.

## Requirements and Dependencies

| Tool        | macOS (26.0+)          | Linux (Ubuntu 24.04+)          |
|-------------|------------------------|--------------------------------|
| Ninja       | `brew install ninja`   | `sudo apt install ninja-build` |
| CMake       | `brew install cmake`   | `sudo apt install cmake`       |
| Clang++-20+ | `brew install llvm@20` | `sudo apt install clang-20`    |
| Metal       | Metal 4                | N/A                            |
| Vulkan      | Vulkan 1.4             | Vulkan 1.4                     |

## CMake

### Flags

- BUILD_SANDBOX: Build the sandbox demo projects
- USE_METAL: Build with Metal (macOS)
- USE_VULKAN: Build with Vulkan (All platforms)

## Examples

### Drawing a triangle in less than 80 lines with Metal

Much of the setup regarding the renderer and windowing system is abstracted allowing you to focus on creating your
custom render loops. To define your own render loop, we need to create our own shader class and implement
`encodeRenderCommand`.

```c++
**Reworking rendering pipeline**
```

## Dependencies

| Library   | Description       | License     |
|-----------|-------------------|-------------|
| GLFW3     | Window Management | zlib/libpng |
| GLM       | Mathematics       | MIT         |
| stb_image | Image Loading     | MIT         |
