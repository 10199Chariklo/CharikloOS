#ifndef CK_TERMINAL_INT_H
#define CK_TERMINAL_INT_H

/* Standard C11 Libraries */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Chariklo Subsystem */
#include "CKtypes_int.h"

#define CK_MAX_CANVAS_COLS 256
#define CK_MAX_CANVAS_ROWS 256

// Immutable 17th Blit Character representing true transparency layer pass-through
#define BLIT_TRANSPARENT_CHAR 'g'

/**
 * @brief Decoupled Structural Memory Buffers.
 * Stores both our virtual composition workspace grid and our flat display output surface.
 */
typedef struct {
    int viewportWidth;
    int viewportHeight;
    
    /* 1. VIRTUAL BUFFER LAYER: Raw target storage grids where nodes commit data streams */
    char virtualChar[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS];
    char virtualText[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS];
    char virtualBg[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS];

    /* 2. PHYSICAL VIEW LAYER: The final composited results mapping virtual boxes to screen space */
    char physicalChar[CK_ROWS][CK_COLS];
    char physicalText[CK_ROWS][CK_COLS];
    char physicalBg[CK_ROWS][CK_COLS];
} CKmasterTerminalBuffer;

extern CKmasterTerminalBuffer CKmasterBuffer;

// Helper definition to access the active window frame seamlessly
#define g_current (CKgetTerminal(NULL))

/**
 * @brief Resolves a safe internal window pointer. If an explicit handle is passed,
 *        it decodes it. If NULL is passed, it falls back to the active tracking state.
 * @param handle Opaque public context window handle or NULL.
 * @return A validated internal CKWindowNode structure reference.
 */
CKwindowHandle CKgetTerminal(CKwindowHandle handle);

void CKterminal_InitMasterBuffer(void);

/**
 * @brief Allocates and registers a new structural tracking node within the viewport tree topology.
 * @param parent Opaque handle mapping to the parent node context frame or NULL.
 * @param bounds Spatial blueprint coordinates specifying position and dimensions.
 * @return A validated internal CKwindowNode structure reference on success, or NULL.
 */
CKwindowNode* CKwindow_Create(CKwindowHandle parent, CKrect bounds);

void CKterminal_CascadeDirty(CKwindowHandle root);

void CKterminal_FreeWindowNode(CKwindowHandle node);

/**
 * @brief Targeted primitive layout engine. Commits raw data text streams 
 *        directly onto a specific bounded window context coordinates.
 * @param handle Validated internal CKWindowNode reference.
 * @param buffer Raw source text byte stream pointer.
 * @param count Total number of characters to process.
 * @return Total number of bytes successfully bounded and committed.
 */
/* STAGE 1: BUFFER LAYER - Commits raw data relative to a node's virtual space box */
int CKfwritef(CKwindowNode* win, const void* buffer, size_t count);

/* STAGE 2: VIEW LAYER - Traverses the hierarchy ring, mapping virtual slices into physical bounds */
void CKterminal_CompositeTree(CKwindowNode* root);

/**
 * @brief Composites the active layout window trees into the master buffer,
 *        then processes and prints the ANSI escape streams out to the terminal.
 */
/* STAGE 3: COMPOSITOR LAYER - Platform-specific terminal painter (ANSI streams) */
void CKplatformRefresh(void);


/**
 * @brief Traverses and serializes the complete active state matrices of both 
 *        the virtual composition boundaries and flat physical view surfaces.
 * @param filename The output filepath target (e.g., "chariklo_render_dump.txt").
 */
void CKterminal_DumpState(const char* filename);

/* Place this inside include/CKterminal_int.h */

/**
 * @brief Internal core primitive to clear or fill a window node context slice.
 * @param node Validated pointer to the target window layout node.
 * @param tBlit The Blit character to apply to the text plane (e.g. '0' or 'g').
 * @param bBlit The Blit character to apply to the background plane (e.g. 'f' or 'g').
 * @param line The relative 0-indexed local row to clear, or -1 to clear the entire node segment.
 * @return True on successful buffer clear, false if node context layout is invalid.
 */
bool CKclearContext_int(CKwindowNode* node, char tBlit, char bBlit, int line);

/**
 * @brief Macro wrapper providing pseudo-optional argument defaults for line segments.
 *        If called with 3 arguments, it defaults line to -1 (Full screen wipe).
 */
#define CLEAR_CONTEXT_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define CKclearContext(...) \
    CLEAR_CONTEXT_MACRO(__VA_ARGS__, CKclearContext_4, CKclearContext_3)(__VA_ARGS__)

#define CKclearContext_3(node, tBlit, bBlit)       CKclearContext_int(node, tBlit, bBlit, -1)
#define CKclearContext_4(node, tBlit, bBlit, line) CKclearContext_int(node, tBlit, bBlit, line)

#endif /* CK_TERMINAL_INT_H */