#ifndef CK_TYPES_INT_H
#define CK_TYPES_INT_H

/* Standard C11 Libraries */
#include <stdbool.h>
#include <stddef.h>

/* Chariklo subsystem */
#include "CKtypes.h"

/* Platform Requirements Type Block (PR) */
#if defined(_WIN32) || defined(_WIN32_)
#include <windows.h>
#include <stdio.h> // Pull in host file pointers under hosted bounds

// Platform requirements struct for Windows
typedef struct CKplatformReqs {
    HANDLE ckSTD_INPUT;
    HANDLE ckSTD_OUTPUT;
    HANDLE ckSTD_ERROR;
    WORD winMask[16];
    COLORREF winColor[16];
} CKplatformReqs;
#else
// Placeholder structure layout for future target operating environments (Linux, Bare-metal)
typedef struct CKplatformReqs {
    int platformInputHandle;
    int platformOutputHandle;
    int platformErrorHandle;
} CKplatformReqs;
#endif

// Globally visible platform requirements register
extern CKplatformReqs PR;

typedef enum {
    CK_STREAM_STDIN,
    CK_STREAM_STDOUT,
    CK_STREAM_STDERR,
    CK_STREAM_DISK_FILE
} CKstreamType;

// --- Unified System File/Stream Handles ---
// Centralizing this object layout enables seamless future integration with CKfs
struct CKfile {
    CKstreamType streamType;
    bool isOpen;
    char mode[4]; // e.g., "r", "w", "a", "rb"
    
#if defined(_WIN32) || defined(_WIN32_)
    FILE* hostFilePointer;
#else
    void* hostFilePointer; // Abstract block for freestanding storage sectors
#endif
};

// --- Modernized Window Server Topology Node ---
// Moving this here allows other core modules to manipulate layout primitives.
struct CKwindowNode {
    /* Dual-Bounded Window Tracking */
    CKrect windowBufferPos; /* Virtual source box within the composition buffer */
    CKrect windowPos;       /* Physical target viewport box on the active screen */
    CKpoint windowSize;     /* Local Width | Height tracking coordinates */

    /* Cursor Tracking Vector */
    CKpoint cursorPos;
    bool cursorBlink;

    /* Blit Color Tracking Registers */
    unsigned int activeTextColor;
    unsigned int activeBackgroundColor;

    /* Render Cache Optimizations */
    bool isDirty;
    bool isVisible;

    /* Branch-Leaf Hierarchy: Doubly Linked Window Server Rings */
    CKwindowHandle parent;
    CKwindowHandle firstChild;
    CKwindowHandle prevSibling;
    CKwindowHandle nextSibling;
};
typedef struct CKwindowNode CKwindowNode; // Clean internal shorthand type alias

// --- Chariklo Centralized OS Environment States (_ENV Equivalents) ---
struct CKenv{
    CKwindowNode* nativeWindowNode;   // Core fallback desktop chassis node
    CKwindowNode* currentWindowNode;  // Context-directed active drawing focus
    bool termPending;                 // Hardware synchronization requested downstream
    unsigned int activeSystemTick;    // Simulated OS lifecycle elapsed counter
};
// Globally visible state registers
extern CKenv CK_ENV;

#endif /* CK_TYPES_INT_H */