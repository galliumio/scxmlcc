[Home](../README.md) | [Documentation](index.md) | [Getting Started](getting-started.md)

# Building scxmlcc on Windows

These steps match a **Visual Studio Build Tools** + **CMake** + **vcpkg** setup (no full Visual Studio IDE required). Tested with **VS 2026** Build Tools; adjust the CMake generator name if yours differs.

## Prerequisites

1. **Build Tools for Visual Studio 2026** (or current year) with workload **Desktop development with C++**.
2. **CMake** for Windows — install from [cmake.org](https://cmake.org/download/) and add CMake to **PATH**.
3. **Git** (for vcpkg and this repository).
4. **vcpkg** — clone once, for example:

   ```powershell
   cd $env:USERPROFILE\Documents\Projects
   git clone https://github.com/microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   ```

## Install Boost (vcpkg)

scxmlcc links `filesystem`, `system`, and `program_options`, and includes header-only Boost (`property_tree`, `algorithm`, etc.). Install:

```powershell
cd $env:USERPROFILE\Documents\Projects\vcpkg
.\vcpkg install boost-filesystem:x64-windows boost-system:x64-windows boost-program-options:x64-windows boost-property-tree:x64-windows boost-algorithm:x64-windows
```

Alternatively install the full port: `.\vcpkg install boost:x64-windows` (larger download).

Note the toolchain file path (used below):

```text
%USERPROFILE%\Documents\Projects\vcpkg\scripts\buildsystems\vcpkg.cmake
```

## Configure and build scxmlcc

Open **x64 Native Tools Command Prompt for VS 2026** (so `cl.exe` is on PATH).

```bat
cd path\to\scxmlcc
mkdir build
cd build

cmake .. -G "Visual Studio 18 2026" -A x64 -DCMAKE_TOOLCHAIN_FILE=%USERPROFILE%/Documents/Projects/vcpkg/scripts/buildsystems/vcpkg.cmake

cmake --build . --config Release --target scxmlcc
```

**Generator name:** If configure fails, run `cmake --help` and use the **Visual Studio … 2026** line listed there, or omit `-G` and let CMake pick the newest installed toolset.

**Output executable:**

```text
build\src\Release\scxmlcc.exe
```

### Optional: run `scxmlcc` from any directory

Add the **directory** containing `scxmlcc.exe` to your user **Path** environment variable (not the `.exe` itself). Open a new terminal after changing Path.

## Running scxmlcc (cmd vs PowerShell)

| Shell | In the folder that contains `scxmlcc.exe` | With Path configured |
|--------|-------------------------------------------|----------------------|
| **cmd** (Native Tools) | `scxmlcc.exe --help` | `scxmlcc --help` |
| **PowerShell** | `.\scxmlcc.exe --help` | `scxmlcc --help` |

PowerShell does not run programs from the current directory unless you prefix with `.\` (security). cmd searches the current directory for `*.exe` names.

## Hello World — generate and run

The sources live under `src/examples/`: `hello_world.scxml`, `hello_world.cpp`, and a checked-in generated header used by some builds. Use this flow to verify the compiler you built.

### 1. Generate the header (manual)

From **cmd** or PowerShell (use `.\` in PowerShell if needed):

```bat
cd path\to\scxmlcc\src\examples
path\to\scxmlcc\build\src\Release\scxmlcc.exe -o hello_world.h hello_world.scxml
```

You should get `hello_world.h` in `src/examples/`. That file is **generated**; commit it only if you intend to track codegen output in git.

### 2. Build and run the Hello World program (CMake)

Reconfigure the **same** `build` directory with examples enabled, then build the example target:

```bat
cd path\to\scxmlcc\build

cmake .. -G "Visual Studio 18 2026" -A x64 -DCMAKE_TOOLCHAIN_FILE=%USERPROFILE%/Documents/Projects/vcpkg/scripts/buildsystems/vcpkg.cmake -DDEVELOPER=1

cmake --build . --config Release --target hello_world
```

`-DDEVELOPER=1` enables examples (including Hello World). On Windows, unit tests are **not** configured (googletest download is unsupported here per upstream).

CMake runs `scxmlcc` automatically for `hello_world` via `scxmlcc_generator` (see `src/examples/CMakeLists.txt`).

Run:

```bat
src\examples\Release\hello_world.exe
```

Expected output (order may follow the statechart):

```text
hello
world
```

### Using scxmlcc in other projects (e.g. STM32CubeIDE)

- **Input:** `.scxml` (from scxmlgui or hand-edited), not UMLet `.uxf`.
- **On change:** run `scxmlcc -o YourChart.h YourChart.scxml` on the PC, add the `.h` to the firmware project, build in the IDE.
- **Optional:** STM32CubeIDE pre-build step calling `scxmlcc.exe` with full paths.

The MCU never runs `scxmlcc`; only the generated C++ is compiled into firmware.
