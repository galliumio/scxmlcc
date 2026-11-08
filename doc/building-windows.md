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

### Gallium Hello World (CMake)

Gallium-specific examples live under `src/examples/gallium/` (same chart names as upstream, e.g. `hello_world.scxml`, but generated with `--gallium`). After configuring with `-DDEVELOPER=1`:

```bat
cd path\to\scxmlcc\build
cmake --build . --config Release --target gallium_hello_world
src\examples\gallium\Release\gallium_hello_world.exe
```

Expected output:

```text
hello
world
```

Codegen runs via `scxmlcc_generator` with `CLIARGUMENTS --gallium` (see `src/examples/gallium/CMakeLists.txt`). Generated in the build dir: `hello_world.h` and `hello_world_event_map.h` (host demo uses only the machine header).

Manual generate (e.g. for STM32CubeIDE):

```bat
cd path\to\scxmlcc\src\examples\gallium
path\to\scxmlcc\build\src\Release\scxmlcc.exe --gallium -i hello_world.scxml -o hello_world.h
```

### Gallium simple_act (keyboard host demo)

**SimpleAct**-style chart under compound **`root`** (`stopped` → `starting` → `started` → `stopping` → `stopped`; **`fault`** on stop failure). Chart id **`simple_act`** → generated **`sc_simple_act`**. Source of truth: `src/examples/gallium/simple_act/simple_act.scxml`; `simple_act.uxf` mirrors the chart for UMLet. The host demo prints each transition’s event, source, and target, plus diagram `/ action` text where present.

```bat
cmake --build . --config Release --target gallium_simple_act
src\examples\gallium\simple_act\Release\gallium_simple_act.exe
```

Keys: **s** start, **p** stop, **d** done, **f** failed, **q** quit. Each key calls `dispatch_int(&state::event_…)` (no scxmlcc queues).

Codegen runs via `scxmlcc_generator` with `CLIARGUMENTS --gallium` (see `src/examples/gallium/simple_act/CMakeLists.txt`). Generated in the build dir: `simple_act.h` and `simple_act_event_map.h` (host demo uses only the machine header; firmware uses both — see event map section below). Not committed; see `src/examples/gallium/.gitignore`.

Manual generate (e.g. for STM32CubeIDE):

```bat
cd path\to\scxmlcc\src\examples\gallium\simple_act
path\to\scxmlcc\build\src\Release\scxmlcc.exe --gallium -i simple_act.scxml -o simple_act.h
```

If `-o` is omitted, the machine header defaults to **`{input-stem}.h`** (same as non-Gallium). With `--gallium`, a second file **`{output-stem}_event_map.h`** is written next to the machine header (e.g. `simple_act_event_map.h` beside `simple_act.h`).

Do not check in generated `{stem}.h` / `{stem}_event_map.h`; add a pre-build step that runs scxmlcc in your firmware project.

### Gallium profile (`--gallium`)

On this fork, generated code always calls **`transition_actions<…>::run`** (upstream uses **`enter`**). Hand-written specializations in examples and firmware must use **`::run`** to match. Fork-specific codegen is marked in **`src/cpp_output.cpp`** with comments prefixed **`// gallium -`** (event map, `dispatch_int`, default internal transitions, and related helpers).

Compared to the default profile, `--gallium` omits scxmlcc `deque` event queues and does not bootstrap through `dispatch_ext()`. Your framework drives the chart: call `init()` once at startup (generated code runs `dispatch_int(&state::initial)` for the SCXML initial transition); for each later stimulus, call `dispatch_int` with the matching `&state::event_…` member pointer from your signal map. Internal events are not generated from SCXML `<raise>`; post them from your framework in C++ (`transition_actions` / `state_actions` specializations in the component `.cpp`, not inline `<script>` in SCXML).

**Default transition type:** with `--gallium`, a `<transition>` without a `type` attribute is generated as `internal` (other profiles keep the SCXML default, `external`). This is the same as writing `type="internal"` on every transition. It only changes transitions whose target is a descendant of the source state (for example a `root` → `stopping` transition on a compound `root`): the source state is not exited and re-entered. Transitions between siblings, targetless transitions and initial transitions are unaffected. Add `type="external"` to a single transition to get the standard external behavior.

Not compatible with:

- **SCXML `<raise>`** — ignored with a codegen warning; use framework calls in C++ template specializations instead.
- **`--baremetal`** — flat state machines only; Gallium charts use hierarchical states.
- **`--threadsafe`** — adds mutexes, condition variables, and `dispatch_loop()` around scxmlcc’s external event queue. `--gallium` omits those queues; you queue events and call `dispatch_int` from one task per chart (or apply your own locking).
- **`--stringevents`** — string `event_map` and `dispatch(name)`; Gallium uses member-pointer events and your signal table.
- **`--namespace` (`-n`)** — not supported with `--gallium`; event map API is `<@name>_evt::type_to_handler` and the generated machine stays global `sc_<name>`.
- **Final states** — not supported in this profile yet (`done.state.*` still uses the internal queue upstream).

#### Event map (`{output-stem}_event_map.h`)

With `--gallium`, each run writes the machine header from **`-o`** (or `{input-stem}.h` by default) and **`{output-stem}_event_map.h`** in the same directory:

```text
scxmlcc --gallium -i simple_act.scxml -o simple_act.h
```

| Item | Example |
|------|---------|
| Machine header (`-o`) | `simple_act.h` |
| Event map (derived from `-o` stem) | `simple_act_event_map.h` |
| Model forward decl + `user_model` | `SimpleActModel` |
| Event-map namespace (`<@name>_evt`) | `simple_act_evt::type_to_handler` |
| Includes in event map | `{output-basename}.h`, `{Pascal}Interface.h`, `{Pascal}Model.h`, `{Pascal}.h` |

SCXML transition events use **PascalCase** names copied into the map. **Interface** events must be prefixed with **PascalCase(`@name`)** (e.g. chart `simple_act` → prefix **`SimpleAct`**, events **`SimpleActStartReq`**, … → **`app::evt::`**). **Private** events (no prefix, e.g. **`Done`**, **`Failed`**) map to **`SimpleAct::`**. **`constexpr kEventMap[]`** of **`EventHandler`** `{ type, handler }` + linear lookup. Bind **`type_to_handler`** in **`ScxmlAdapter`**. **`fw::Evt::type`** holds the id (webserver uses string **`type`**).

Stem of `-i` should match `@name` (warning if not).

### Using scxmlcc in other projects (e.g. STM32CubeIDE)

- **Input:** `.scxml` (from scxmlgui or hand-edited), not UMLet `.uxf`.
- **On change:** run `scxmlcc --gallium -i YourChart.scxml -o your_chart.h` on the PC (also writes `your_chart_event_map.h`), add both generated headers to the firmware project, build in the IDE.
- **Optional:** STM32CubeIDE pre-build step calling `scxmlcc.exe` with full paths.

The MCU never runs `scxmlcc`; only the generated C++ is compiled into firmware.
