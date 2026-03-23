# tyshi-engine

This engine is a byproduct of building my 2D survival game tyshi. I extracted this as a reusable API but it is heavily specified for the game. This is a WIP. 

## Requirements

Tools and libraries required to build and run the engine. 

Can only be built on Linux, targeting both Linux and Windows. I have not tested building on Windows (It might be possible with changes in [CMakeLists.txt](CMakeLists.txt)).  

### Tools 

- `g++` - Compiling the source files
- `pkg-config` - Finding packages and getting their flags for `cmake`
- `cmake` -  Building, linking and setting compiler flags 
- `make` - Running `cmake` commands
- `mingw-w64`
    - Cross-compiling to **64-bit Windows** (The engine code has only been tested in **64-bit Windows** but the toolchain can be changed if targeting different platform and/or architecture. For standalone builds you have to provide a different toolchain file in the [Makefile](Makefile).)

### Libraries

Libraries and their versions used during development.

| Library | Version |
|---------|---------|
| SDL3 | 3.2.4 |
| SDL3_image | 3.2.0 |
| SDL3_ttf | 3.1.0 |

## Use as a submodule

The engine can be used as a submodule in other git repos. This is the main reason I separated the engine and game code. The engine will be built as a static library and linked with the game code in the root repo.

To add to other repos:

```bash
git submodule add https://github.com/pdridh/tyshi-engine engine/
git submodule update --init
```

Now for the main part, the root repo needs it's own **CMakeLists.txt** to build and link the engine as a static library.

In the root repo's **CMakelists.txt**, `CMAKE_INSTALL_PREFIX` must be set before `add_subdirectory()` because the engine's [`CMakeLists.txt`](CMakeLists.txt) uses it to install the engine assets to the root repo's target path:

```cmake
if(CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin/windows" CACHE PATH "" FORCE)
else()
    set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}/bin/linux" CACHE PATH "" FORCE)
endif()

# Add engine 
add_subdirectory(engine/)

# SDL3 and all other engine dependencies are inherited automatically
target_include_directories(game-target PRIVATE engine/include/)
target_link_libraries(game-target PRIVATE tyshi-engine)
```

The game code can access the Engine's API by including them as `"Engine/File.h"`, for example:

```cpp
#include "Engine/Engine.h"
#include "Engine/Vec2.h"
```

---

### Building for Windows

`cmake` requires a toolchain file for cross compiling, so it must be provided otherwise it will build for Linux automatically. The toolchain file must set these at least:

| CMake Variable | Recommended |
|---------|---------|
| `CMAKE_SYSTEM_NAME` | Windows |
| `CMAKE_SYSTEM_PROCESSOR` | x86_64 |
| `CMAKE_C_COMPILER` | x86_64-w64-mingw32-gcc |
| `CMAKE_CXX_COMPILER` | x86_64-w64-mingw32-g++ |
| `CMAKE_FIND_ROOT_PATH` | Wherever you installed your target system files and libraries  |
| `ENV{PKG_CONFIG_PATH}` | Path to the pkgconfig of the toolchain you are using |

The engine is responsible for installing and adding it's own required dlls to the target executable's directory. It uses the pkgconfig set above to find the dlls.


## Standalone Build

For testing and/or developing, the engine can be built as a standalone project. If `cmake` is ran with this repository as the root then it uses the files in [test/](test/) to build an executable.

The [Makefile](Makefile) has targets that can be used to easily run `cmake` commands with the appropriate flags set based on the target.

### Building for Linux

For a debug build use `make dev` this builds a debug binary and also runs it. For a release build use `make release`.

The binary can be found in **bin/linux/** to change this modify [Makefile](Makefile)

---

### Building for Windows

For a debug build use `make win-dev`, this builds a debug executable, gets the required dlls from the path set in [CMake/mingw-toolchain.cmake](CMake/mingw-toolchain.cmake) and installs it in the same directory as the executable. The debug build opens a console window.

For a release build use `make win-release`. It does pretty much the same as for debug build except there is no console window.

The binary can be found in **bin/windows/** to change this modify [Makefile](Makefile)

## Credits

- **[SDL3](https://github.com/libsdl-org/SDL)** — windowing, rendering, input and platform abstraction
- **[SDL3_image](https://github.com/libsdl-org/SDL_image)** — image loading
- **[SDL3_ttf](https://github.com/libsdl-org/SDL_ttf)** — font rendering
- **[JetBrains Mono](https://www.jetbrains.com/lp/mono/)** — bundled as the engine's default debug font, licensed under the [SIL Open Font License](assets/fonts/OFL.txt)
- **[Doxygen](https://www.doxygen.nl/)** - generating docs 