# tyshi-engine

This engine is a byproduct of building my 2D survival game tyshi. I extracted this as a reusable API but it is heavily specified for the game. This is a WIP. 

## Requirements

Tools and libraries required to build and run the engine. 

Can only be built on Linux, targeting both Linux and Windows. I have not tested building on Windows.  

### Tools 

- `g++` - Compiling the source files
- `mingw-w64`- Cross-compiling to windows 
- `pkg-config` - Finding packages and getting their flags for `cmake`
- `cmake` -  Building, linking and setting compiler flags 
- `make` - Running `cmake` commands

### Libraries

Libraries and their versions used during development.

| Library | Version |
|---------|---------|
| SDL3 | 3.2.4 |
| SDL3_image | 3.2.0 |
| SDL3_ttf | 3.1.0 |


## Setup 

Before using this as a submodule or building it as a standalone executable, there are some things to setup. Obviously, everything in the requirements must be installed.

At the time of writing this, you cannot install the SDL3 via package managers like `apt` as it is very new. So it must be manually installed.

For Linux, you have to build SDL3 from source.

If cross-compiling for Windows then the appropriate Mingw SDL3 must also be installed, getting it from SDL3's releases is probably the easiest way. `cmake` expects a toolchain file for it to know that it is building for Windows. In the [Makefile](Makefile) the toolchain file path is set to [CMake/mingw-toolchain.cmake](CMake/mingw-toolchain.cmake). This toolchain file sets the Windows libraries path as **/usr/local/x86_64-w64-mingw32** and pkgconfig file for mingw as **/usr/local/x86_64-w64-mingw32/lib/pkgconfig**.

Therefore, MinGW SDL3 must be installed to the expected path:
```
/usr/local/x86_64-w64-mingw32/
├── bin/          - SDL3.dll, SDL3_image.dll, SDL3_ttf.dll
├── include/      - SDL3 headers
└── lib/
    └── pkgconfig/ - sdl3.pc, sdl3-image.pc, sdl3-ttf.pc
```

So, you either need the libraries and pkgconfig file in that path or you can modify the toolchain file. If using a different toolchain file, set `TOOLCHAIN_FILE` in [Makefile](Makefile) to it's filepath. 


## Use as a submodule

The engine can be used as a submodule in other git repos. This is the main reason I separated the engine and game code. The engine will be built as a static library and linked with the game code in the root repo.

To add to other repos:

```bash
git submodule add https://github.com/pdridh/tyshi-engine engine/
git submodule update --init
```

Now for the main part, the root repo needs it's own `CMakeLists.txt` to build and link the engine as a static library.

`CMAKE_INSTALL_PREFIX` must be set before `add_subdirectory()` because the engine's [`CMakeLists.txt`](CMakeLists.txt) uses it to install the engine assets to the root repo's target path.

In the root repo's `CMakeLists.txt`:
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

When building for windows, it requires a toolchain file, it is recommended to use the engine's [toolchain file](CMake/mingw-toolchain.cmake) as the toolchain file when building the game too. See [Building for windows](#building-for-windows).


The game code can access the Engine's API by including them as `"Engine/File.h"`, for example:

```cpp
#include "Engine/Engine.h"
#include "Engine/Vec2.h"
```

## Building

For testing and/or developing the engine can be built as a standalone project. If `cmake` is ran with this repository as the root, then, it uses the files in [test/](test/) to build an executable.

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