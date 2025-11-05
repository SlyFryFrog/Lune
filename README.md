# Lune

## Dependencies

| Tool       | macOS (26.0+)      | Linux (Ubuntu 24.04+)        |
|------------|--------------------|------------------------------|
| Ninja      | brew install ninja | sudo apt install ninja-build |
| CMake      | brew install cmake | sudo apt install cmake       |
| Vcpkg      |                    |                              |
| Clang++-20 |                    |                              |
| Metal      | Metal 4            | N/A                          |
| Vulkan     | Vulkan 1.4         | Vulkan 1.4                   |



## CMake

### Flags

- BUILD_SANDBOX: Build the sandbox demo projects
- USE_METAL: Build with Metal (macOS)
- USE_VULKAN: Build with Vulkan (All platforms)