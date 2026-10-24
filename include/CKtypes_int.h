#ifndef CK_TYPES_INT_H
#define CK_TYPES_INT_H

/* Standard C11 Libraries */
#include <stdbool.h>
#include <stddef.h>

/* Chariklo subsystem */
#include "CKtypes.h"

/* Platform Requirements Type Block (PR) */
//
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
extern CKplatformReqs PR; // Globally visible platform requirements register

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


/* --- Dynamic Acrylic Graph engine | CKgraphUtils structures --- */
// 
struct CKgraphNodeHeader {
    struct CKgraphNodeHeader* parent;
    struct CKgraphNodeHeader* firstChild;
    struct CKgraphNodeHeader* prevSibling;
    struct CKgraphNodeHeader* nextSibling;
};
typedef struct CKgraphNodeHeader CKgraphNodeHeader;

/* --- 0(2) Buffer + Compositer engine | CKterminal_int enums --- */
// Chariklo Viewport Layout Traversal Flags
typedef enum {
    CK_CASCADE_DIRTY   = (1 << 0),  // Bit 0: Force re-render evaluation flag (0x01)
    CK_CASCADE_HIDE    = (1 << 1),  // Bit 1: Forces visibility state to false (0x02)
    CK_CASCADE_SHOW    = (1 << 2)   // Bit 2: Forces visibility state to true  (0x04)
} CKcascadeFlags;

/* --- Modernized Window Server Topology Node --- */
//
struct CKwindowNode {
    /**
     * INTRUSIVE GRAPH ENGINE ANCHOR
     * Must remain the first member! This guarantees that a pointer to a 
     * CKwindowNode can be directly safe-cast to a CKgraphNodeHeader pointer.
     */
    CKgraphNodeHeader graphNode;

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
    
};
typedef struct CKwindowNode CKwindowNode;

// --- Chariklo Centralized OS Environment States (_ENV Equivalents) ---
//
struct CKenv{
    CKwindowNode* nativeWindowNode;   // Core fallback desktop chassis node
    CKwindowNode* currentWindowNode;  // Context-directed active drawing focus
    bool termPending;                 // Hardware synchronization requested downstream
    unsigned int activeSystemTick;    // Simulated OS lifecycle elapsed counter
};
extern CKenv CK_ENV; // Globally visible state registers

#endif /* CK_TYPES_INT_H */