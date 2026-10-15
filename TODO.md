# CharikloOS Development Roadmap & Module Pipeline

Because our core architecture is designed to be highly modular, our codebase is divided into clear milestones. This roadmap outlines the remaining primitives needed to achieve functional parity with ComputerCraft's CraftOS, along with our custom subsystems for advanced terminal graphics and physics-based UI manipulation.

---

## 🏛 Contributor Integration Matrix: Library Architecture

Not every module fits neatly into a standard template. If you are importing external codebases or writing platform-specific extensions, structure your module using the boundary rules below to match our architectural system:

[ External Library / Raw File ]  <-- e.g., PhysicsFS, custom asset packs
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

*   **`module_platform.c` (The Ground):** Isolates native system anomalies (e.g., Win32 `AllocConsole`, raw framebuffer pipes) so the rest of the code remains fully portable.
*   **`module_core.c` (The Engine):** Implements internal algorithms and manages memory buffers using raw primitives.
*   **`module.h` (The Interface):** Exposes clean, **Opaque Struct Handles** to users, keeping underlying implementation details hidden.

---

## 🚀 The Core Module Pipeline

### 1. Foundational Data Structures & Math Primitives
*   **`CKdeque` (Double-Ended Queue):** High-speed, ring-buffered queue structure for low-overhead keyboard scan-code tracking, event dispatching, and thread-safe pipeline stream scheduling.
*   **`CKrectUtils` (Bounding & Cell Group Geometry):** Standardized framework for grid-aligned structural math. Used by `CKwindow` to calculate clipping bounds, and available to users as a fast sprite/tile movement controller.
*   **`CKvelocity` (Kinetic Engine Primitives):** Physics-based math engine to apply fluid, consistent movement to cursor tracking, terminal scrolling, and window snapping. Combined with `CKrectUtils`, this provides an incredibly fluid UI workspace.

### 2. High-Fidelity Terminal Compositor & Palettes
*   **Sub-Pixel Blending Pipeline:** Replaces standard 16-color restrictions with a real-time mask-blending pipeline. By interlacing or masking the base 16 colors during execution, the engine expands the terminal space into **65,000 perceived colors**.
    *   *Modes:* High-fidelity Regular Blending and Hue-Locked Monochromatic Blending.
*   **Advanced Color Palettes:** Complete layout registries for:
    *   `Colodore` (Commodore 64 optimized palette rules)
    *   Linear Greyscale maps
    *   Retro Amber and Green phosphor monochrome matrices

### 3. CraftOS Parity Components (High-Tier Rings)
*   **`CUparallel` / `CUtos`:** High-level abstractions enabling co-routine style function execution mappings and multi-process shell threading.
*   **`CUgps` & `CUpaint`:** Modular UI extensions for spatial localization simulations and terminal-native bitmap artwork generation.
*   **`CUvector` / `CUtextutils`:** Specialized string serialization, tabulating engines, and hardware-independent rendering layouts.

---

## ⚡ The Security Layer (Advanced System Intercepts)
*   **`CKdirectPhysicsFS_int` / `CKfatFS_int`:** Low-level, un-sandboxed disk access drivers. These internal modules completely bypass the standard `/.computerX/` jail boundaries for raw administrative host tasks. 
*   *Note:* These modules intentionally omit shell auto-fill routines to prevent accidental system destruction by users, though they remain fully linkable for systems development.