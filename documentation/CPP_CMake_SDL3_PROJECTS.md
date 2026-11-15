# C++ / CMake / SDL3 — Best Practice Project Structure

As your game grows, a simple `main.cpp` won't be enough. Here's how professional C++ game projects are typically organized.

## Recommended Structure

```
my-game/
├── CMakeLists.txt              # Root build configuration
├── README.md
├── src/                        # All source code
│   ├── main.cpp               # Entry point (initializes SDL, starts game loop)
│   ├── game/
│   │   ├── Game.cpp           # Main game class
│   │   └── Game.h
│   ├── entities/
│   │   ├── Paddle.cpp         # Paddle game object
│   │   ├── Paddle.h
│   │   ├── Ball.cpp           # Ball game object
│   │   └── Ball.h
│   ├── rendering/
│   │   ├── Renderer.cpp       # Drawing utilities
│   │   └── Renderer.h
│   └── input/
│       ├── InputHandler.cpp   # Keyboard/mouse input
│       └── InputHandler.h
├── assets/                     # Game assets (images, sounds, fonts)
│   ├── images/
│   ├── sounds/
│   └── fonts/
├── tests/                      # Unit tests
│   ├── CMakeLists.txt
│   └── test_paddle.cpp
├── build/                      # Compiled output (ignore in version control)
└── .gitignore                  # Tells Git to ignore build/ and other files
```

## Key Principles

| Principle | Why It Matters |
|-----------|---------------|
| **Separate `src/` and `build/`** | Keeps your clean source code separate from messy compiled files. |
| **One `.cpp` + `.h` pair per class** | Each game object (Paddle, Ball, etc.) gets its own files for organization. |
| **`main.cpp` stays small** | It should only initialize SDL and start the game — all logic goes in other files. |
| **`assets/` for media** | Images, sounds, and fonts live outside the code. |
| **`tests/` for testing** | Automated tests ensure your code works as you add features. |
| **`.gitignore`** | Prevents compiled files and secrets from being uploaded to version control. |

## How CMake Scales with More Files

When you have multiple source files, `CMakeLists.txt` grows slightly:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_game LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
find_package(SDL3 REQUIRED)

# Collect all source files
file(GLOB SOURCES "src/*.cpp" "src/**/*.cpp")

# Create the executable
add_executable(my_game ${SOURCES})

# Link SDL3
target_link_libraries(my_game PRIVATE SDL3::SDL3)

# Include the src directory so #include "game/Game.h" works
target_include_directories(my_game PRIVATE src)
```

---

See also: [`PROJECT_STRUCTURE.md`](PROJECT_STRUCTURE.md) for your current project layout · [`GODOT_COMPARISON.md`](GODOT_COMPARISON.md) for how this compares to Godot.
