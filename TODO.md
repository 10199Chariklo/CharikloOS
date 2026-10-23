# CharikloOS Development Roadmap & Module Pipeline

Because our core architecture is designed to be highly modular, our codebase is divided into clear milestones. This roadmap outlines the remaining primitives needed to achieve functional parity with ComputerCraft's CraftOS, along with our custom subsystems for advanced terminal graphics and physics-based UI manipulation.


---
## 🚀 The Core Module Pipeline

### 1. Foundational Data Structures & Math Primitives
*   **`CKdeque` (Double-Ended Queue):** High-speed, ring-buffered queue structure for low-overhead keyboard scan-code tracking, event dispatching, and thread-safe pipeline stream scheduling.
*   **`CKrectUtils` (Bounding & Cell Group Geometry):** Standardized framework for grid-aligned structural math. Used by `CKwindow` to calculate clipping bounds, and available to users as a fast sprite/tile movement controller.
*   **`CKvelocity` (Kinetic Engine Primitives):** Physics-based math engine to apply fluid, consistent movement to cursor tracking, terminal scrolling, and window snapping. Combined with `CKrectUtils`, this provides an incredibly fluid UI workspace.
*   **`CKgraphUtils` (Universal Directed Acyclic Graph Engine):** Generic, type-agnostic `void*` pointer traversal framework. Standardizes system tree operations (Pre-Order, Post-Order, In-Order) across multiple modules to drive unified window compositing, Z-index handling, input focus routing, and memory sub-tree cleanups.

### 2. Advanced Graph, 3D Rasterization & Entity Subsystems
*   **`CKbspUtils` (Binary Space Partitioning & Tiling Layout Engine):** Spatial decomposition utility that translates generic tree hierarchies into perfectly fitted, non-overlapping window configurations. Automatically computes and scales flat user interface grids inside parent boundaries.
*   **`CKmeshTopology_int` (Topological Mesh Decoder & Stream Verifier):** Low-level asset pipeline that calculates and validates the **Euler Characteristic (χ = V - E + F)** of 3D meshes on ingestion. Guarantees structural surface integrity over virtual storage or network feeds before processing data down-chain.
*   **`CKquadRender_int` (Squared-Rectangle Quad-Edge 3D Pipeline):** Advanced software rasterizer that converts 3D models into flat 2D rectangular layouts derived via **Brooks' Theorem (Planar Smith Diagrams)**. Maps mesh coordinates to textures with zero alignment padding, using sequenced edge truncation and horizontal/vertical line rectification to project high-performance perspective geometry straight onto target `CKwindowNode` cell matrices.
*   **`CKentities` (Object-Bound Visual Effects & Collision Framework):** Object-oriented particle and trigger system anchored directly to window vectors (centered or pinned to one of four corners). Uses `CKvelocity` fixed-point calculations to drive complex kinetic effects (smoke/waves) directly into character matrices. Utilizes `CKdeque` pipelines to interact bidirectionally: objects can broadcast system events via `CKpushBack` on boundary hit detections (`CKrectUtils`), or listen via `CKpopFront` to activate drawn behaviors.

### 3. High-Fidelity Terminal Compositor & Palettes
*   **Sub-Pixel Blending Pipeline:** Replaces standard 16-color restrictions with a real-time mask-blending pipeline. By interlacing or masking the base 16 colors during execution, the engine expands the terminal space into **65,000 perceived colors**.
    *   *Modes:* High-fidelity Regular Blending and Hue-Locked Monochromatic Blending.
    *   *3D Synergy:* Delivers smooth, artifact-free lighting gradients and ambient occlusion shadows for the quad rendering engine.
*   **Advanced Color Palettes:** Complete layout registries for:
    *   `Colodore` (Commodore 64 optimized palette rules)
    *   Linear Greyscale maps
    *   Retro Amber and Green phosphor monochrome matrices (with simulated CRT intensity attenuation decay trails)

### 4. CraftOS Parity Components (High-Tier Rings)
*   **`CUparallel` / `CUtos`:** High-level abstractions enabling co-routine style function execution mappings and multi-process shell threading.
*   **`CUgps` & `CUpaint`:** Modular UI extensions for spatial localization simulations and terminal-native bitmap artwork generation.
*   **`CUvector` / `CUtextutils`:** Specialized string serialization, tabulating engines, and hardware-independent rendering layouts.


---
## ⚡ The Security Layer (Advanced System Intercepts)
*   **`CKdirectPhysicsFS_int` / `CKfatFS_int`:** Low-level, un-sandboxed disk access drivers. These internal modules completely bypass the standard `/.computerX/` jail boundaries for raw administrative host tasks. 
*   *Note:* These modules intentionally omit shell auto-fill routines to prevent accidental system destruction by users, though they remain fully linkable for systems development.


---
## 📊 Infrastructure Mapping Matrix

### Module Progression & Dependency Table

| Module Name | Tier Prefix | Upstream Prerequisites | Immediate Downstream Beneficiaries |
| :--- | :--- | :--- | :--- |
| **`CKgraphUtils`** | `CK` (Foundational primitive) | *None* (Pure pointer logic) | `CKwindow`, `CKbspUtils`, `CKmeshTopology_int` |
| **`CKbspUtils`** | `CK` / `OU` (Layout wrapper) | `CKgraphUtils`, `CKrectUtils` | Flat automated UI setup workflows, `CKquadRender_int` |
| **`CKmeshTopology_int`** | `CK_int` (Internal data driver) | `CKgraphUtils` | 3D Asset pipeline validation hooks |
| **`CKquadRender_int`** | `CK_int` (Internal graphics driver) | `CKbspUtils`, `CKrectUtils`, `CKvelocity` | 3D Workspace sandboxes, game environments |

### CKentities System Integration Blueprint

| Entity Component | Upstream Dependencies | Core Engine Function | Downstream Sandbox Value |
| :--- | :--- | :--- | :--- |
| **Anchor Map** | `CKrectUtils` | Tracks offsets relative to parent window boundaries. | Zero-overhead particle anchoring during window movements. |
| **Particle Physics** | `CKvelocity` | Integrates fixed-point kinetic steps to individual cell glyphs. | Smooth fluid, smoke, or explosion animations across character grids. |
| **Trigger Matrix** | `CKdeque` | Broadcasts or captures event tokens via queue hooks. | Interactive UI badges, animated window alerts, and built-in text-adventure game logic. |