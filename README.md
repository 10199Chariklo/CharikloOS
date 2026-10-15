# CharikloOS 🪐
An ultra-lightweight, context-aware operating system simulation framework written in pure C11.

CharikloOS reimagines the elegant, scriptable workspace paradigm of **ComputerCraft's CraftOS**, translating its interface mechanics into high-performance, statically typed **C11 idioms**. It is designed to act as a dual-purpose environment: an educational ramp for students learning systems programming, and a high-speed sandboxed workspace for hobbyist OS developers

Also - Welcome to my *first* ever public release of anything, and *first* time using Github! Welcome to Chariklo, I love you.


---
## 🚀 The Vision: Freedom of the Ringless API

Unlike traditional operating systems that enforce strict hardware execution rings to isolate applications from the kernel, CharikloOS features **zero functional rings restricting execution**. 

There is no arbitrary barrier between "user space" and "kernel space." Instead, the system implements an **educational ramp**: APIs naturally expand in scope, power, and granularity the deeper you dive into the framework. 

### Sandboxing over Bricking
To protect users from accidentally destroying their environments, security is managed purely at the storage boundary:
*   **The Public Sandbox:** Standard applications are strictly confined to their localized `/.computerX/` virtual roots and configuration defaults via the public `CKfs` api.
*   **The Direct Primitives:** Low-level subsystems capable of altering the broader disk structure (such as un-sandboxed file tools or physical sector drivers) exist under explicit internal namespaces (e.g., `CKdirectPhysicsFS_int` or `CKfatFS_int`). 
*   **Shell Obscurity:** These internal `_int` modules do **not** auto-fill within the command shell. This hides them from novice users while keeping them fully discoverable and includable for developers who intentionally require deep system control.


---
## ☄️ Project Nomenclature: The Centaur & The Rings
Inspired by the **Centaur 10199 Chariklo** and its unique double-ring system, the project enforces a structural architectural mapping across all files and functions. This design instantly communicates a file's dependency depth and structural footprint at a glance.

### 1. File Dependency Prefix Tiers

The repository is stratified into three explicit naming tiers based on library inclusion boundaries:

*   **`CK` (The Body / System Primitives):** 
    *   Files that include **any external libraries** (e.g., `<stdio.h>`, `<windows.h>`, or `physfs.h`).
    *   *OR* files that are **completely self-contained**, including absolutely zero libraries.
*   **`OU` (The First Ring / Core Logic):** 
    *   Files that include **only** files prefixed with `CK`. They are entirely insulated from external host libraries.
*   **`CU` (The Second Ring / Scripting & Utility Logic):** 
    *   Files that include a combination of `CK` and `OU` files, *or* include **only** files prefixed with `OU`.

### 2. Function Prefix Resolution Rules
To maintain an un-cluttered public API namespace while respecting lower-level implementations, function prefixes follow a **Lowest Common Ancestor** rule:

> **The Rule:** A function's name prefix is dictated by the lowest-tier prefix present anywhere within its entire compilation module.

*   *Example:* If a newly built module "addition" is composed of a high-level interface file `OUaddition.h` paired with an underlying implementation core file `CKaddition_core.c`, **every function in that module uses the `CK` prefix** (e.g., `CKcomputeSum()`). Even if the function fingerprint is declared inside an `OU` header, its execution dependency on a `CK` core module forces the prefix down to the foundational tier.


---
## 🛠 Project Architecture & Directory Layout
CharikloOS uses a strictly segmented directory structure designed for modular scalability and clean cross-platform builds:

```text
CharikloOS/
├─ CMakeLists.txt              # Automated build orchestration contract
├─ src/                        # Internal core kernel module engines
│   └── CKterminal/            # CKterminal module source Directory
│       ├── OUterminal_core.c  # CKterminal system agnostic implement
│       └── CKterminal_win.c   # CKterminal _WIN32 wrapper | Windows implement
├─ include/                    # Public publicly exposed subsystem interface hubs
│   └── CKterminal_int.h       # CKterminal Master buffer / Terminal manipulators
|     (Notice the _int, CKterminal is an internal API, with 'ramped' capabilities)
└─ unittest/                   # Isolated independent testing harnesses
    └── RevisionTest04.c       # One of our unit tests / applications
```


---
## Windows Powershell Quickstart: Building via Clang + Ninja
CharikloOS is optimized for modern **LLVM/Clang-LLD toolchains**. This quickstart guide uses **MSYS2** on Windows to automate compilation.

### 0. Install MSYS2 and CMAKE
Include CMake in your enviroment PATH during installation - It's an option during installion - Don't miss it!

MSYS2 does **not** need to be in your path if you follow our example and point strait to the executables.

### 1. Prerequisites (Via MSYS2 Clang64 Shell)
Ensure you have the required compilers and build generators installed via `pacman`:
```bash
pacman -S mingw-w64-clang-x86_64-clang mingw-w64-clang-x86_64-ninja
```

### 2. Compilation
Download the repository, open your PowerShell, and execute our automatic compilation steps:

```powershell
# Navigate to the CharikloOS directory you just obtained
cd Downloads/CharikloOS

# Create an isolated build directory
# ALWAYS start with an empty build directory
mkdir build
cd build

# Configure the pure C workspace using the explicit compiler paths
cmake .. -G "Ninja" -DCMAKE_C_COMPILER="C:/msys64/clang64/bin/clang.exe" -DCMAKE_MAKE_PROGRAM="C:/msys64/clang64/bin/ninja.exe"

# Execute the compilation build target
cmake --build .
```

Upon a successful build, the automated system outputs an optimized binary straight to your `build/bin/` folder. Run it to launch whichever RevisionTest or build is active!

**CRITICAL NOTE:**  YOU SHOULD NEVER RUN CODE LIKE THIS ON YOUR MACHINE YOU DO NOT TRUST.

For absolute safety when compiling code from any new open-source project, you should always:
  - **Inspect the source code** prior to compilation.
  - **Scan the source files** and workspace assets with updated antivirus software.
  - **Execute all compilation steps and final executables in a secure virtual machine(VM)**
    - Even better; Do this in an air-gapped / fresh OS installation with no accounts or personal information registered at setup.

Pinky swear I'm not trying to hi-jack your computer - But if you are new to compilation, understand what compilation chains are building, and the power native machine code has.


---
## META
Documents created with LLM aid - Expanded from early project Jot notes and edited / expanded upon further.

I try to write as much code as I can - But an LLM's ability to refactor the project quickly after I make a module with sweeping implications? Unbeatable. I also at least run free options or locally when I need it, and never with a harness (Yet..?). Mostly treating them as tutors in my prompts, so I can get analogies and running examples, or handing them single modules at a time for refactors. You can do this with Google for EG.

This is my *first* major project in C - A gross scope expansion from originally wanting to make a basic Garbage Collector and learn how to make better programs for Lua / CraftOS - Now I'm making a CraftOS inspired OS to learn better systems programing and really trying to step into C. Shoot for the moon they say. Either I have become a star, or landed with the *centaurs*. 