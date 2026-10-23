# Contributing to CharikloOS
CharikloOS is an unrestricted, modular operating system simulation framework. Whether you see it as a retro-scriptable workspace, a freestanding bare-metal arcade engine, or a lightweight application sandboxing runtime—**your architectural vision is welcome here.**

You do not have to follow a rigid development track to make an impact. If you want to build a feature, design its module architecture, slot it in, and submit a pull request! We don't believe in forcing creative scripts or native user programs into identical boxes. Instead, we challenge you to plan your own isolated systems while leveraging our structural standards to increase the likelihood of a seamless upstream merge.

---
## 🧭 The Core Philosophies

To keep the project clean, fast, and stable, all contributions must align with our core design pillars:

1. **Unrestricted Freedom (Ringless Execution):** We do not arbitrarily lock features or create artificial execution barriers. APIs ramp up in scope as developers go deeper. If a module needs low-level hardware or raw host storage access, provide it—just follow the naming conventions so users know what they are invoking.
2. **Logical Scope Progression:** Codebases mature sequentially. We love ambitious features, but they must land when the runtime can actively support them *(e.g., please do not submit a high-tier media codec before the audio mixing primitives are written)*.
3. **Zero-Copy C11 Idioms:** Prioritize modern, high-efficiency C11 execution footprints. Avoid heavy runtime string allocations, streaming copies, or hidden garbage collection. Rely on direct-address memory grids, explicit lifecycle contexts, and opaque reference handles.
4. **Sandboxed by Default:** Standard user-facing tools must respect the virtual storage limits of the `/.computerX/` boundaries via `CKfs`. If your module intentionally breaks out of the sandbox to perform direct host operations, use the internal `_int` namespace suffix so it doesn't auto-fill or accidentally get triggered by a novice user.


---
## 🏛 Suggested Module Structure

Not every contribution fits a strict template—especially if you are submitting standalone user programs, lightweight utilities, or shell scripts built on top of the environment. However, if you are importing complex external codebases or writing low-level engine extensions, we highly encourage you to isolate your system boundaries using this structural roadmap:

    [ External Library / Raw Input ]  <-- e.g., PhysicsFS, custom asset packs, raw streams
                │
                ▼
    ┌───────────────────────┐
    │   module_platform.c   │  <-- Handles OS-specific hooks (Win32, Posix, Bare-Metal)
    └───────────┬───────────┘
                │
                ▼
    ┌───────────────────────┐
    │    module_core.c      │  <-- Core memory allocation and hardware-agnostic states
    └───────────┬───────────┘
                │
                ▼
    ┌───────────────────────┐
    │       module.h        │  <-- Exposes clean, Opaque Handles to the application layer
    └───────────────────────┘

*   **`module_platform.c` (The Ground):** Isolates native system anomalies (e.g., Win32 `AllocConsole`, raw framebuffer pipes) so the rest of your logic remains fully portable.
*   **`module_core.c` (The Engine):** Implements internal algorithms and manages memory buffers using raw primitives.
*   **`module.h` (The Interface):** Exposes clean, **Opaque Struct Handles** to users, keeping underlying implementation details hidden.


---
## ☄️ Naming & Structural Conventions
To keep system footprints completely visible at a glance, native framework source assets and functional API signatures should map onto our **Centaur and Rings** prefix matrix.

### File Prefix Tiers
* **`CK` (The Body):** Core files incorporating external native host dependencies (e.g., `<stdio.h>`, `<windows.h>`, `physfs.h`) OR files that remain completely freestanding with zero `#include` footprints.
* **`OU` (The First Ring):** Middle-tier modules containing internal system logic that include **only** underlying `CK` headers. They are completely insulated from host OS environments.
* **`CU` (The Second Ring):** Scripting, utility, and user-space wrappers relying on mixed underlying `CK` and `OU` trees, or **only** `OU` layers.

### Function Prefix Resolution Rule
APIs resolve namespaces via a **Lowest Common Ancestor** compilation rule. A function’s exported name prefix is dictated by the lowest-tier prefix present anywhere within its underlying translation unit.
* *Example:* If an interface header is named `OUmy_module.h` but its underlying algorithmic implementation executes within a source core asset named `CKmy_core.c`, **every exposed function in that module must use the `CK` prefix** (e.g., `CKmy_function_name()`).



---
## 🛠 Architectural Safety Guardrails
Because CharikloOS is a space for learning C11 memory management, we enforce code conventions that prevent common memory bugs before they happen:

* **Opaque Struct Handles:** Never expose raw structures in public headers if they manage internal memory buffers. Pass them as opaque pointers (e.g., `typedef struct CK_ENV* CK_ENV_HANDLE;`). This forces developers to use explicit constructor/destructor functions (`CKinitEnv()`, `CKfreeEnv()`) and prevents dangling stack allocations.
* **Explicit Array Boundaries:** Never expose a raw pointer array parameter inside a functional signature without anchoring an explicit tracking size parameter directly next to it.
* **No `file(GLOB)` Compilations:** Register explicit source compilation manifests cleanly inside the central `CMakeLists.txt` build contract rather than relying on loose directory scans.


---
## 🚀 How to Submit a Contribution
1. **Fork & Clone:** Mirror the repository on GitHub and pull your fork down locally.
2. **Verify Natively:** Confirm your target sandbox compiles natively using our standard MSYS2 Clang/Ninja configuration steps highlighted in the main `README.md`.
3. **Isolate Your Proof:** Build an independent testing framework inside the `unittest/` folder using the `RevisionTestXX.c` system convention to ensure your logic functions correctly without causing regressions.
4. **Open a Pull Request:** Link your feature branch! Outline the module's target tier depth, its structural parameters, and detail how to successfully evaluate your specific `unittest` runner.