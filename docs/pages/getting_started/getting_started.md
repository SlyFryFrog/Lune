# Getting Started

Before getting into the steps to setup a project with Lys, there are several that must first be addressed.

## Prerequisites

- **CMake 3.28**
- **Ninja**
- **Clang 19**
- **Vcpkg**

## Building the Project

1. Clone the Repository

```bash
git clone git@github.com:SlyFryFrog/Lys.git
cd Lys
```

2. Configure CMakeUserPresets

Before running the preset, it is necessary to configure the `CMakeUserPresets.json`. An example of what this may look like on linux is as follows.

```json
{
  "version": 8,
  "configurePresets": [
    {
      "name": "config",
      "inherits": "abstract",
      "hidden": true,
      "environment": {
        "VCPKG_ROOT": "PATH"
      },
      "cacheVariables": {
        "CMAKE_C_COMPILER": "/usr/bin/clang-19",
        "CMAKE_CXX_COMPILER": "/usr/bin/clang++-19"
      }
    },
    {
      "name": "DebugLinux",
      "inherits": ["config", "Debug"]
    },
    {
      "name": "ReleaseLinux",
      "inherits": ["config", "Release"]
    }
  ]
}
```

## Build with a CMakeUserPreset

To configure and generate the build files:

```bash
cmake --preset DebugLinux
```

or 

```bash
cmake --preset ReleaseLinux
```

Then build with:

```bash
cmake --build build/Debug
```
