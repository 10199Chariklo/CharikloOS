#ifndef CK_TERMINAL_INT_H
#define CK_TERMINAL_INT_H

/* Standard C11 Libraries */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Chariklo Subsystem */
    /* Distant module headers */
#include "CKtypes.h"
#include "CKwindow.h"

#define CK_MAX_CANVAS_COLS 256
#define CK_MAX_CANVAS_ROWS 256

// Immutable 17th Blit Character representing true transparency layer pass-through
#define BLIT_TRANSPARENT_CHAR 'g'

typedef struct {
    int viewportX;
    int viewportY;
    int viewportWidth;
    int viewportHeight;
    char charBuffer[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS];
    char textBlitBuffer[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS];
    char bgBlitBuffer[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS];
} CKmasterTerminalBuffer;
extern CKmasterTerminalBuffer CKmasterBuffer;

// Unified Composition Header sharing topological and hierarchical state
struct CKwindowNode {
    /* Window Tracking */
    CKrect windowPos;
    CKpoint windowSize; // Width | Height pair

    /* Cursor Tracking */
    CKpoint cursorPos;
    bool cursorBlink;

    /* Color Tracking */
    unsigned int activeTextColor;
    unsigned int activeBackgroundColor;

    /* Refresh Tracking */
    bool isDirty;
    bool isVisible;

    /* Branch-Leaf Hierarchy and Doubly Linked Window Server Rings */
    CKwindowNode* parent;
    CKwindowNode* firstChild;
    CKwindowNode* prevSibling;
    CKwindowNode* nextSibling;
};

// Helper definition to access the active window frame seamlessly
#define g_current (CKgetTerminal(NULL))

// Master internal tracking references for the Window Server Rings
extern CKwindowNode* g_NativeWindowNode;
extern CKwindowNode* g_CurrentWindowNode;

/**
 * @brief Resolves a safe internal window pointer. If an explicit handle is passed,
 *        it decodes it. If NULL is passed, it falls back to the active tracking state.
 * @param handle Opaque public context window handle or NULL.
 * @return A validated internal CKWindowNode structure reference.
 */
CKwindowNode* CKgetTerminal(CKwindowHandle handle);

void CKterminal_InitMasterBuffer(void);
CKwindowNode* CKwindow_Create(int x, int y, int width, int height);
void CKterminal_CascadeDirty(CKwindowNode* root);
void CKterminal_FreeWindowNode(CKwindowNode* node);

/**
 * @brief Composites the active layout window trees into the master buffer,
 *        then processes and prints the ANSI escape streams out to the terminal.
 */
void CKplatformRefresh(void);

/**
 * @brief Targeted primitive layout engine. Commits raw data text streams 
 *        directly onto a specific bounded window context coordinates.
 * @param handle Validated internal CKWindowNode reference.
 * @param buffer Raw source text byte stream pointer.
 * @param count Total number of characters to process.
 * @return Total number of bytes successfully bounded and committed.
 */
int CKfwritef(CKwindowNode* handle, const void* buffer, size_t count);

#endif // CK_TERMINAL_INT_H