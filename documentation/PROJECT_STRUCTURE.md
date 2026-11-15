# Project Structure Guide

## For Complete Beginners

This file explains how all the pieces of your game project fit together — from the code you write to the program you run.

---

## Your Current Project Layout

```
pj-games/
├── CMakeLists.txt          # Build instructions (the "recipe")
├── README.md               # Project description
├── src/
│   └── main.cpp            # Your game code (the "ingredients")
├── build/                  # Compiled output (the "cake")
│   └── sdl3_demo           # The runnable program
└── PROJECT_STRUCTURE.md    # This file!
```

---

## How It All Connects: The Build Pipeline

Think of building a program like baking a cake:

### Step 1: You write the recipe (`CMakeLists.txt`)

`CMakeLists.txt` is a set of instructions that tells the computer **how** to build your project. In your file, these are the key lines:

```cmake
project(sdl3_demo LANGUAGES CXX)          # "This is a C++ project"
find_package(SDL3 REQUIRED)               # "Find the SDL3 library on my system"
add_executable(sdl3_demo src/main.cpp)    # "Compile main.cpp into a program called sdl3_demo"
target_link_libraries(sdl3_demo PRIVATE SDL3::SDL3)  # "Connect the SDL3 library to the program"
```

### Step 2: You write the ingredients (`src/main.cpp`)

This is where you write your actual game logic — the code that creates windows, draws graphics, handles input, and runs the game loop.

### Step 3: CMake reads the recipe and generates build files

When you run `cmake .` (or when Rider does it automatically), CMake reads `CMakeLists.txt` and creates a set of build instructions in the `build/` folder. It figures out:

- Where your C++ compiler (`g++`) is located
- Where the SDL3 library headers and binaries are installed
- What compiler flags to use

### Step 4: The compiler turns your code into a program

When you run `cmake --build .`, the compiler (`g++`) does two things:

1. **Compiles** `src/main.cpp` → `sdl3_demo.o` (an object file, a middle step)
2. **Links** `sdl3_demo.o` + SDL3 library → `build/sdl3_demo` (the final runnable program)

### Step 5: You run the program

```bash
./build/sdl3_demo
```

This launches your game! The program uses the SDL3 library (installed on your system at `/usr/local/lib/libSDL3.so`) to talk to your computer's graphics system and display the window.

---

## The Role of Each Piece

### `src/main.cpp` — Your Source Code
- **What it is:** Human-readable C++ code that you write and edit.
- **What it does:** Contains all your game logic — creating windows, handling input, drawing graphics.
- **Analogy:** The recipe ingredients (flour, eggs, sugar).

### `CMakeLists.txt` — Build Configuration
- **What it is:** A configuration file that describes your project.
- **What it does:** Tells the build system what to compile, what libraries to use, and what standards to follow.
- **Analogy:** The recipe card that says how to combine the ingredients.

### `build/` — Build Output Directory
- **What it is:** A folder containing all the compiled files.
- **What it does:** Stores intermediate files (`.o` object files) and the final executable.
- **Why a separate folder?** Keeping build files separate from source code keeps your project clean. You can delete the entire `build/` folder and rebuild from scratch without losing any of your code.
- **Analogy:** The kitchen counter where you mix and bake — messy, but separate from your pantry.

### SDL3 Library — The Game Framework
- **What it is:** A pre-written library installed on your system (`/usr/local/lib/libSDL3.so`).
- **What it does:** Provides functions like `SDL_CreateWindow()`, `SDL_CreateRenderer()`, and `SDL_PollEvent()` that handle the hard work of talking to your operating system's graphics, audio, and input systems.
- **How it connects:** When you `#include <SDL3/SDL.h>` in your code, the compiler finds the header files. When you link with `SDL3::SDL3`, the linker connects your code to the actual library functions.
- **Analogy:** A pre-made cake mix — you still need to add eggs and bake it, but the hard work of measuring flour is done for you.

---

## Quick Reference: Common Commands

| Command | What It Does |
|---------|-------------|
| `cmake -B build` | Configure the project (read `CMakeLists.txt`, prepare build files) |
| `cmake --build build` | Compile the code (turn `.cpp` into the executable) |
| `./build/sdl3_demo` | Run your game |
| `pkg-config --modversion sdl3` | Check which SDL3 version is installed |

---

## Summary

- **`src/main.cpp`** = Your code (what the game does)
- **`CMakeLists.txt`** = Build instructions (how to compile your code)
- **`build/`** = Compiled output (the runnable program)
- **SDL3** = The library that lets your code talk to the computer's hardware
- **CMake** = The tool that reads `CMakeLists.txt` and orchestrates the build

The flow is: **Write code** → **CMake configures** → **Compiler builds** → **You run the game**.

---

## Further Reading

- [`BEST_PRACTICES.md`](BEST_PRACTICES.md) — How to structure your project as it grows
- [`GODOT_COMPARISON.md`](GODOT_COMPARISON.md) — How C++ + SDL compares to the Godot engine
