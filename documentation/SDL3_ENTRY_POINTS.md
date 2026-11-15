# SDL3 Entry Points: `main()` vs Callbacks

This guide compares the two ways to structure an SDL3 program: the traditional `main()` (SDL_main) approach and the callback-based approach (`SDL_MAIN_USE_CALLBACKS`). It also covers related SDL3 features that affect how your program starts and runs.

---

## 1. Traditional `main()` / `SDL_main` Approach

### How It Works

You write a standard `main()` function. SDL3 wraps it behind the scenes as `SDL_main`, which handles platform-specific entry points (such as `wWinMain` on Windows) automatically. You have full, explicit control over the entire application lifecycle.

### Code Structure

Everything lives in one function:

```cpp
#include <SDL3/SDL.h>
#include <iostream>

int main(int argc, char* argv[])
{
    // 1. Initialization
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL3 Hello World", 640, 480, 0);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // 2. Main loop
    bool running = true;
    SDL_Event event;
    while (running)
    {
        // Event handling
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                running = false;
            }
        }

        // Rendering
        SDL_SetRenderDrawColor(renderer, 50, 120, 255, 255);
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);
    }

    // 3. Cleanup
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "SDL3 program exited successfully!" << std::endl;
    return 0;
}
```

### Pros

- **Full explicit control** — you own the loop, the timing, and the flow.
- **Widely used** — most SDL3 tutorials, examples, and documentation use this pattern.
- **Clear mental model** — initialization, loop, and cleanup are visible in one place.
- **Simpler for learning** — easier to debug and understand program flow.

### Cons

- **More boilerplate** — you write the event loop and loop condition yourself.
- **Platform-specific glue** — if targeting consoles or mobile, you may need additional adaptation code.

### Best For

Desktop development, learning SDL3, simple games, and projects where you want maximum visibility over the application lifecycle.

---

## 2. Callback-Based Approach (`SDL_MAIN_USE_CALLBACKS`)

### How It Works

By defining `#define SDL_MAIN_USE_CALLBACKS 1` before including SDL headers, you opt into a callback-driven model. Instead of writing `main()`, you implement four callback functions that SDL calls at the appropriate times. SDL manages the event loop for you.

### Code Structure

The same program, expressed as four callbacks:

```cpp
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <iostream>

int SDL_AppInit(void** appstate, int argc, char* argv[], void* secrets)
{
    // Initialization
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDL3 Hello World", 640, 480, 0);
    if (!window)
    {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Store pointers for other callbacks via appstate
    struct AppState
    {
        SDL_Window* window;
        SDL_Renderer* renderer;
    };

    *appstate = new AppState{window, renderer};
    return 0;
}

int SDL_AppEvent(SDL_App* app, const SDL_Event* event)
{
    // Event handling — called per event
    if (event->type == SDL_EVENT_QUIT)
    {
        return 1;  // Return non-zero to quit
    }
    return 0;
}

int SDL_AppIterate(SDL_App* app)
{
    // Each frame of the main loop
    auto* state = static_cast<AppState*>(app->appstate);

    SDL_SetRenderDrawColor(state->renderer, 50, 120, 255, 255);
    SDL_RenderClear(state->renderer);
    SDL_RenderPresent(state->renderer);

    return 0;  // Return 0 to continue, non-zero to quit
}

int SDL_AppQuit(SDL_App* app, int exitcode, void* secrets)
{
    // Cleanup
    auto* state = static_cast<AppState*>(app->appstate);
    SDL_DestroyRenderer(state->renderer);
    SDL_DestroyWindow(state->window);
    delete state;
    SDL_Quit();

    std::cout << "SDL3 program exited successfully!" << std::endl;
    return exitcode;
}
```

### Pros

- **Less boilerplate** — SDL manages the event loop internally.
- **Platform lifecycle handled** — SDL adapts the callback pattern to each platform's expectations.
- **Better for non-desktop** — consoles, mobile, and web platforms work naturally with this model.

### Cons

- **Code split across four functions** — harder to see the full flow at a glance.
- **Less common in tutorials** — fewer examples and learning resources use this pattern.
- **SDL owns the loop** — less direct control over timing and iteration.

### Best For

Cross-platform projects, console/mobile/web targets, and developers who prefer SDL to manage the application loop.

---

## 3. Side-by-Side Comparison

| Feature | Traditional `main()` | Callbacks |
|---|---|---|
| **Who owns the loop** | You (explicit `while` loop) | SDL (calls your callbacks) |
| **Code structure** | One `main()` function | Four callback functions |
| **Boilerplate** | More (you write the loop) | Less (SDL manages it) |
| **Tutorial availability** | Very common, most examples | Less common |
| **Platform portability** | Desktop-first; needs glue for others | Built-in platform adaptation |
| **Learning curve** | Lower — straightforward flow | Slightly higher — four functions to track |
| **State sharing** | Local variables in `main()` | Via `appstate` pointer in `SDL_App` |
| **Exit control** | `return` from `main()` | Return non-zero from callbacks |
| **Official SDL stance** | "Primary way" SDL expects | "Completely optional" alternative |

---

## 4. Mapping Between Patterns

Switching between the two patterns is essentially cutting and pasting code into four functions. Here's how each section maps:

| Traditional `main()` section | Callback function |
|---|---|
| Code before the loop (`SDL_Init`, `SDL_CreateWindow`, `SDL_CreateRenderer`, etc.) | `SDL_AppInit()` |
| `SDL_PollEvent()` loop and event handling | `SDL_AppEvent()` |
| Inside `while(running)` — rendering, game logic, updates | `SDL_AppIterate()` |
| Code after the loop (`SDL_Destroy*`, `SDL_Quit()`) | `SDL_AppQuit()` |

Converting your existing `main()` to callbacks typically takes about 2–5 minutes and requires no logic changes — just moving blocks of code into the appropriate callback.

---

## 5. Related SDL3 Features

### `SDL_MAIN_USE_CALLBACKS` Macro

Place this directive **before** including any SDL headers to enable the callback pattern:

```cpp
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
```

Without this macro, SDL defaults to the traditional `SDL_main` (which resolves to your `main()` function).

### The `SDL_App` Struct

The `SDL_App` struct is passed to `SDL_AppEvent`, `SDL_AppIterate`, and `SDL_AppQuit`. It contains:

- `argc` and `argv` — command-line arguments.
- `appstate` — a `void*` pointer you set in `SDL_AppInit` to store any data (window, renderer, game state, etc.) that needs to be shared across callbacks.

### `SDL_AppInit` Parameters

```cpp
int SDL_AppInit(void** appstate, int argc, char* argv[], void* secrets);
```

- `appstate` — set this output pointer to store your application state for later callbacks.
- `argc` / `argv` — command-line arguments.
- `secrets` — platform-specific secrets (e.g. credentials on some mobile platforms); usually `nullptr` on desktop.
- **Return value** — return `0` for success, non-zero to abort initialization.

### `SDL_AppEvent` Parameters

```cpp
int SDL_AppEvent(SDL_App* app, const SDL_Event* event);
```

- Called once per event (unlike `SDL_PollEvent`, which you call in a loop).
- **Return value** — return non-zero to signal the application should quit.

### `SDL_AppIterate` Parameters

```cpp
int SDL_AppIterate(SDL_App* app);
```

- Called once per frame by SDL's internal loop.
- **Return value** — return `0` to continue, non-zero to quit.

### `SDL_AppQuit` Parameters

```cpp
int SDL_AppQuit(SDL_App* app, int exitcode, void* secrets);
```

- Called when the application is shutting down.
- `exitcode` — the exit code passed from the quitting callback.
- **Return value** — conventionally, return the `exitcode` unchanged.

### `SDL_main` and Platform Entry Points

On most platforms, `SDL_main` resolves directly to your `main()` function. However, on platforms with different entry point conventions (e.g. Windows `wWinMain`, macOS `UIApplicationMain`), SDL provides the platform-specific wrapper so your `SDL_main` still works. This is transparent — you just write `main()` and SDL handles the rest.

---

## 6. Recommendations

### For Beginners and Desktop-Only Development

**Stick with `main()`.** It's the most widely used pattern, appears in most tutorials and examples, and gives you a clear, linear view of your program's flow. There's no disadvantage to using it for desktop games.

### For Cross-Platform Ambitions

**Consider callbacks.** If you plan to target consoles, mobile, or web, the callback pattern lets SDL handle platform lifecycle differences automatically. You still write the same logic — it's just organized differently.

### Switching Is Trivial

Moving between the two patterns is a mechanical exercise: cut code from `main()` and paste it into the four callback functions. No logic changes are required. If your needs change later, switching will take about five minutes.

### Focus on What Matters

The entry point pattern is a minor structural detail. More important skills for game development include:

- Game architecture and state management
- Rendering pipelines and texture handling
- Input handling and event processing
- Physics, collision detection, and AI
- Asset management and resource loading

Master these first. The choice between `main()` and callbacks won't affect your growth as a game developer.
