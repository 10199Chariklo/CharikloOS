#ifndef CK_TYPES_H
#define CK_TYPES_H

/* Standard C11 Libraries */
#include <stdbool.h>
#include <stddef.h>

/* Platform Requirements Type Block (PR) */
// Unified platform registry type layout.
#if defined(_WIN32) || defined(_WIN32_)
#include <windows.h>

#include <stdio.h> // Pull in host file pointers under strict hosted bounds

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
// Globally visible hardware state register instance
extern CKplatformReqs PR;

/* Chariklo viewport Boundaries */
#ifndef CK_COLS
#define CK_COLS 80
#define CK_ROWS 30
#endif

typedef enum {
    CK_STREAM_STDIN,
    CK_STREAM_STDOUT,
    CK_STREAM_STDERR,
    CK_STREAM_DISK_FILE
} CKstreamType;

/* Chariklo Spatial Abstractions */
typedef struct {
    int x;
    int y;
} CKpoint;

typedef struct {
    int startX;
    int startY;
    int endX;
    int endY;
} CKrect;

// Chariklo Core Numeric Type Definitions
typedef unsigned char      ck_u8;
typedef unsigned short     ck_u16;
typedef unsigned int       ck_u32;

// --- Abstract System Type Registry ---
// Forward declaration of the window node structure.
// This allows modules to reference the pointer while leaving implementation opaque.
typedef struct CKwindowNode CKwindowNode;

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

// --- Chariklo Centralized OS Environment States (_ENV Equivalents) ---
typedef struct {
    CKwindowNode* nativeWindowNode;   // Core fallback desktop chassis node
    CKwindowNode* currentWindowNode;  // Context-directed active drawing focus
    bool termPending;                 // Hardware synchronization requested downstream
    unsigned int activeSystemTick;    // Simulated OS lifecycle elapsed counter
} CKenv;
// Globally visible state registers
extern CKenv CK_ENV;

/**
 * @brief Idiomatic, safe string length checker bounded by an upper limit.
 * @return Number of characters before null terminator or limit.
 *
size_t CKstrnlen(const char* str, size_t maxLen);

 * @brief High-performance integer-to-string formatter. Replaces basic snprintf loops.
 * @param value The signed integer to convert.
 * @param buffer Output target character grid array.
 * @param bufSize Maximum boundary size of the target buffer.
 * @return True if conversion fits entirely inside bounds.
 *
bool CKitoa(int value, char* buffer, size_t bufSize);   */

#endif // CK_TYPES_H