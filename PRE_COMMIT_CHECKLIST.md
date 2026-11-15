# Pre-Commit Checklist

Before you run `git commit`, use this checklist to make sure your documentation stays in sync with your code.

## Quick Checklist

| Check | What to update |
|-------|---------------|
| Did I add or remove a `.cpp`/`.h` file? | Update the **Project Structure** table in `README.md` |
| Did I change `CMakeLists.txt`? | Update the **Prerequisites** or **Building** section if needed |
| Did I add a new folder (e.g., `assets/`, `tests/`)? | Add it to the table in `README.md` and `PROJECT_STRUCTURE.md` |
| Did I change how the game works? | Update the **Current Status** line in `README.md` |

## Detailed Guide

### 1. `README.md`

This is the first file anyone reads. Keep it accurate:

- **Current Status** — Does it still describe what the project does?
- **Project Structure table** — Does every file/folder listed actually exist? Are any new ones missing?
- **Prerequisites** — Did you add a new dependency (e.g., a new library)?
- **Building/Running** — Did the build or run commands change?

### 2. `PROJECT_STRUCTURE.md`

This explains your current project layout in detail:

- **Your Current Project Layout** — Update the tree diagram if you added/removed files or folders.
- **The Role of Each Piece** — Add entries for any new components (e.g., `assets/`, `tests/`).

## When to Switch to Automation

When your project has **20+ files** and the table takes more than 30 seconds to update, that's when a pre-commit git hook makes sense. For now, keep it manual — it's a good learning habit.