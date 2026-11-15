# pj-games [Philip Branch]
Fellas building things and learning along the way

### Current Status: 
- A C++ game project using **SDL3**, currently just showing a blue screen but building towards a Breakout game.

## Project Structure

| File / Folder | Purpose |
|---------------|---------|
| `src/main.cpp` | Your game source code |
| `CMakeLists.txt` | Build instructions (tells CMake how to compile) |
| `build/` | Compiled output (do not commit to Git) |

## Documentation

- [`PROJECT_STRUCTURE.md`](PROJECT_STRUCTURE.md) — Beginner's guide: how your code, CMake, and SDL3 connect
- [`BEST_PRACTICES.md`](BEST_PRACTICES.md) — How to structure your project as it grows
- [`GODOT_COMPARISON.md`](GODOT_COMPARISON.md) — How C++ + SDL compares to the Godot engine
- [`PRE_COMMIT_CHECKLIST.md`](PRE_COMMIT_CHECKLIST.md) — What to check before each git commit

## Prerequisites

- **CMake** 3.16+
- **C++17 compiler** (g++ or clang++)
- **SDL3** installed on your system

To check your SDL3 version:
```bash
pkg-config --modversion sdl3
```

## Building

```bash
cmake -B build          # Configure the project
cmake --build build     # Compile the code
```

## Running

```bash
./build/sdl3_demo
```

This opens a 640×480 window. Close it to exit.
