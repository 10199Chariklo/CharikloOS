# Contributing to CharikloOS
CharikloOS is designed as an unrestricted, modular operating system simulation framework. Whether you see CharikloOS as a retro-scriptable OS environment, a freestanding bare-metal arcade cabinet engine, or a lightweight cross-platform application framework—**your vision is welcome here.**

You do not have to stick to an official "To-Do" list to make a contribution. If you want to build a feature, write a module for it, slot it in, and submit a pull request!


---
## 🧭 The Core Philosophies

To keep the project clean, fast, and stable, all contributions must align with our core design pillars:

1. **Unrestricted Freedom (Ringless Execution):** We do not arbitrarily lock features or create artificial execution barriers. APIs ramp up in scope as developers go deeper. If a module needs low-level hardware or raw host storage access, provide it—just follow the naming conventions so users know what they are invoking.
2. **Logical Scope Progression:** Build what the system can actually use today. We love ambitious features, but they must match the framework's current lifecycle maturity. *(For example: Please don't submit an MKV video codec before our base audio engine is written—we wouldn't have anything to pipe the data into yet!)*
3. **Zero-Copy & Pure C11 Idioms:** We prioritize modern, performance-first C11 layouts. Avoid heavy, streaming string allocations or garbage-collected patterns. Use direct-address memory maps, opaque resource handles, and explicit context routing.
4. **Sandboxed by Default:** Standard user-facing tools must respect the virtual storage limits of the `/.computerX/` boundaries via `CKfs`. If your module intentionally breaks out of the sandbox to perform direct host operations, use the internal `_int` namespace suffix so it doesn't auto-fill or accidentally get triggered by a novice user.


---
## ☄️ Naming & Structure Conventions

Every file and function added to the repository must follow our **Centaur and Rings** prefix matrix. This ensures that any developer can instantly tell a file’s dependency depth and structural footprint at a glance.

### File Prefix Tiers
* **`CK` (The Body):** Files that include external host libraries (e.g., `<stdio.h>`, `<windows.h>`, `physfs.h`) OR files that contain absolutely zero include statements.
* **`OU` (The First Ring):** Files that include **only** other `CK` prefixed files. They are insulated from external host environments.
* **`CU` (The Second Ring):** Files that include `CK` and `OU` files, or **only** `OU` files.

### Function Prefix Rule
Functions use the **Lowest Common Ancestor** rule. A function's name prefix is dictated by the lowest-tier prefix present anywhere within its entire compilation module.
* *Example:* If your header is `OUmy_module.h` but its implementation relies on a foundational file named `CKmy_core.c`, **every single function in that module must use the `CK` prefix** (e.g., `CKmy_function_name()`).


---
## 🛠 Architectural Safety Guardrails
Because CharikloOS is a space for learning C11 memory management, we enforce code conventions that prevent common memory bugs before they happen:

* **Opaque Struct Handles:** Never expose raw structures in public headers if they manage internal memory buffers. Pass them as opaque pointers (e.g., `typedef struct CK_ENV* CK_ENV_HANDLE;`). This forces developers to use explicit constructor/destructor functions (`CKinitEnv()`, `CKfreeEnv()`) and prevents dangling stack allocations.
* **Explicit Array Bounds:** Never pass raw arrays into functions without an accompanying size parameter.
* **No `file(GLOB)` Pollution:** If you add new source folders, ensure they are registered correctly in the central `CMakeLists.txt` without accidentally overwriting existing source collections.


---
## 🚀 How to Submit a Contribution
1. **Fork and Clone:** Fork the repository on GitHub and pull it down to your local machine.
2. **Build and Verify Natively:** Set up your build folder using the explicit MSYS2 Clang/Ninja instructions outlined in the `README.md`.
3. **Isolate Your Feature:** Create a self-contained test file inside the `unittest/` directory (following our `RevisionTestXX.c` convention) to prove your module works independently without breaking existing baseline systems.
4. **Open a Pull Request:** Submit your code! In your PR description, briefly explain your module's purpose, which prefix tier it belongs to, and how to run your unittest file.