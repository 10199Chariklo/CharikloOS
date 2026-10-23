/* Standard C11 Libraries */
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* Chariklo Subsystem */
#include "CKterminal_int.h" // Local module header
#include "CKpalette_int.h"
#include "CKgraphUtils.h"
#include "CKtypes_int.h"
#include "CKhandles.h"

#include "CKrectUtils_int.h"

// Define the global master engine buffer instance
CKmasterTerminalBuffer CKmasterBuffer;

// Concrete initialization instance of the global Chariklo execution environment registers
CKenv CK_ENV = {
    .nativeWindowNode = NULL,
    .currentWindowNode = NULL,
    .termPending = false,
    .activeSystemTick = 0
};

// Next available top-left positioning coordinate track inside our virtual grid block
static int g_nextVirtualX = 0;
static int g_nextVirtualY = 0;

CKwindowNode* CKgetTerminal(CKwindowHandle handle) {
    // If a specific workspace frame is passed explicitly, decode and return it
    if (handle != NULL) return (CKwindowNode*)handle;
    
    // Fallback 1: If an active redirected context is running, return it
    if (CK_ENV.currentWindowNode != NULL) return CK_ENV.currentWindowNode;
    
    // Fallback 2: Ultimate fallback to the base chassis frame configuration
    if (CK_ENV.nativeWindowNode != NULL) return CK_ENV.nativeWindowNode;
    
    // Hard failure recovery: Returns NULL if setup has completely skipped initialization
    return NULL;
}

void CKterminal_InitMasterBuffer(void) {
    CKmasterBuffer.viewportWidth = CK_COLS;
    CKmasterBuffer.viewportHeight = CK_ROWS;

    // Reset virtual blocks
    memset(CKmasterBuffer.virtualChar, CK_GLYPH_VOID, sizeof(CKmasterBuffer.virtualChar));
    memset(CKmasterBuffer.virtualText, BLIT_TRANSPARENT_CHAR, sizeof(CKmasterBuffer.virtualText));
    memset(CKmasterBuffer.virtualBg, BLIT_TRANSPARENT_CHAR, sizeof(CKmasterBuffer.virtualBg));

    // Reset physical view blocks
    memset(CKmasterBuffer.physicalChar, ' ', sizeof(CKmasterBuffer.physicalChar));
    memset(CKmasterBuffer.physicalText, '0', sizeof(CKmasterBuffer.physicalText));
    memset(CKmasterBuffer.physicalBg, 'f', sizeof(CKmasterBuffer.physicalBg));
}

CKwindowNode* CKwindow_Create(CKwindowHandle parent, CKrect bounds) {
    // 1. Allocate block memory segment
    CKwindowNode* node = (CKwindowNode*)malloc(sizeof(CKwindowNode));
    if (node == NULL) return NULL;

    // 2. Map target viewport screen dimensions using our math engine normalizer
    node->windowPos = CKrectNormalize(bounds.startX, bounds.startY, bounds.endX, bounds.endY);
    
    // Derive absolute sizing metrics cleanly from the normalized coordinate entries
    int width  = node->windowPos.endX - node->windowPos.startX;
    int height = node->windowPos.endY - node->windowPos.startY;
    node->windowSize.x = width;
    node->windowSize.y = height;

    // 3. Segment isolated virtual allocation coordinates
    node->windowBufferPos.startX = g_nextVirtualX;
    node->windowBufferPos.startY = g_nextVirtualY;
    node->windowBufferPos.endX   = g_nextVirtualX + width;
    node->windowBufferPos.endY   = g_nextVirtualY + height;

    // Packing stack pointer progression arithmetic
    g_nextVirtualY += height;
    if (g_nextVirtualY >= CK_MAX_CANVAS_ROWS) {
        g_nextVirtualY = 0;
        g_nextVirtualX += width; // Wrap columns safely
    }

    // 4. Initialize structural tracking registries via the embedded Graph Header
    node->graphNode.parent       = NULL;
    node->graphNode.firstChild   = NULL;
    node->graphNode.prevSibling  = NULL;
    node->graphNode.nextSibling  = NULL;

    node->cursorPos.x = 0;
    node->cursorPos.y = 0;
    node->cursorBlink = true;
    node->isDirty     = true;
    node->isVisible   = true;

    // 5. STATEFUL COLOR INHERITANCE INJECTION
    char defaultTextBlit, defaultBgBlit;
    
    if (parent == NULL) {
        node->activeTextColor       = 0x0001; // Default White bitmask code
        node->activeBackgroundColor = 0x8000; // Default Black bitmask code
        defaultTextBlit = '0';
        defaultBgBlit   = 'f';
    } else {
        CKwindowNode* parentNode = (CKwindowNode*)parent;
        node->activeTextColor       = parentNode->activeTextColor;
        node->activeBackgroundColor = parentNode->activeBackgroundColor;
        
        // Virtual buffers remain completely transparent pass-through spaces
        defaultTextBlit = BLIT_TRANSPARENT_CHAR; // 'g'
        defaultBgBlit   = BLIT_TRANSPARENT_CHAR; // 'g'
    }

    // Initialize the window's allocated slice inside the virtual grid arrays
    for (int r = 0; r < height; r++) {
        for (int c = 0; c < width; c++) {
            int vX = node->windowBufferPos.startX + c;
            int vY = node->windowBufferPos.startY + r;

            if (vX >= 0 && vX < CK_MAX_CANVAS_COLS && vY >= 0 && vY < CK_MAX_CANVAS_ROWS) {
                CKmasterBuffer.virtualChar[vY][vX] = CK_GLYPH_VOID;
                CKmasterBuffer.virtualText[vY][vX] = defaultTextBlit;
                CKmasterBuffer.virtualBg[vY][vX]   = defaultBgBlit;
            }
        }
    }

    return node;
}

// 1. Generic Flag Modifier Visitor
static bool CascadeFlagVisitor(CKgraphNodeHandle node, void* context) {
    if (!node) return false;
    
    CKwindowNode* win = (CKwindowNode*)node;
    
    // Recover our composite target flags from the type-agnostic context pointer
    uintptr_t activeFlags = (uintptr_t)context;

    // Condition 1: Evaluate structural dirty modifications
    if (activeFlags & CK_CASCADE_DIRTY) {
        win->isDirty = true;
    }

    // Condition 2: Evaluate layout hide operations
    if (activeFlags & CK_CASCADE_HIDE) {
        win->isVisible = false;
        win->isDirty   = true; // Hiding an item inherently flags the canvas layout as dirty
    }

    // Condition 3: Evaluate layout show operations
    if (activeFlags & CK_CASCADE_SHOW) {
        win->isVisible = true;
        win->isDirty   = true; // Showing an item forces structural redraw sweeps
    }
    
    return true; // Continue the iterative loop traversal
}

// 2. The Upgraded, Non-Recursive Cascade Flag Engine Interface
void CKterminal_CascadeFlag(CKwindowHandle root, unsigned int flags) {
    if (root == NULL || flags == 0) return;
    
    // Cast the raw numeric flag value directly into the context pointer to avoid stack usage
    uintptr_t contextPayload = (uintptr_t)flags;
    
    // Trigger our high-performance non-recursive pre-order flattening pass
    CKgraph_Flatten((CKgraphNodeHandle)root, CK_PRE_ORDER, CascadeFlagVisitor, (void*)contextPayload);
}

void CKterminal_FreeWindowNode(CKwindowNode* node) {
    if (node == NULL) return;

    // 1. Invoke the graph core to safely unbind layout links across siblings and parents
    CKgraph_DetachSubTree((CKgraphNodeHandle)node);

    // 2. Safely release the concrete memory block back to the system heap
    free(node);
    
    printf("[CKgc] Target Window Object successfully collected and freed.\n");
}

/* STAGE 1: THE BUFFER LAYER APPLICATION WRITE */
int CKfwritef(CKwindowNode* win, const void* buffer, size_t count) {
    if (win == NULL || buffer == NULL || count == 0) return 0;
    
    const char* sourceBytes = (const char*)buffer;
    int bytesCommitted = 0;

    // Cache blit mappings to optimize lookup execution overhead loops
    char activeTBlit = '0'; 
    if (win->activeTextColor == BLIT_TRANSPARENT_CHAR) { // 'g'
        activeTBlit = BLIT_TRANSPARENT_CHAR;
    } else {
        for (int i = 0; i < 16; i++) {
            if (CKpalette.data[i].colorID == win->activeTextColor) {
                activeTBlit = CKpalette.data[i].blitChar;
                break;
            }
        }
    }

    // ARCHITECTURAL INTERCEPT: If the window container's state background color is explicitly 
    // configured to use alpha pass-through ('g'), preserve it! Otherwise, perform the palette lookup.
    char activeBBlit = 'f';
    if (win->activeBackgroundColor == BLIT_TRANSPARENT_CHAR) { // 'g'
        activeBBlit = BLIT_TRANSPARENT_CHAR;
    } else {
        for (int i = 0; i < 16; i++) {
            if (CKpalette.data[i].colorID == win->activeBackgroundColor) {
                activeBBlit = CKpalette.data[i].blitChar;
                break;
            }
        }
    }

    for (size_t i = 0; i < count; i++) {
        char character = sourceBytes[i];

        if (character == '\n') {
            win->cursorPos.x = 0;
            win->cursorPos.y++;
            bytesCommitted++;
            continue;
        }

        // Relative local boundary tracking clip check
        if (win->cursorPos.x >= 0 && win->cursorPos.x < win->windowSize.x &&
            win->cursorPos.y >= 0 && win->cursorPos.y < win->windowSize.y) {

            // MAP STRATEGY: Offset relative vectors straight into your isolated virtual buffer block bounds
            int vX = win->windowBufferPos.startX + win->cursorPos.x;
            int vY = win->windowBufferPos.startY + win->cursorPos.y;

            if (vX >= 0 && vX < CK_MAX_CANVAS_COLS && vY >= 0 && vY < CK_MAX_CANVAS_ROWS) {
                CKmasterBuffer.virtualChar[vY][vX] = character;
                CKmasterBuffer.virtualText[vY][vX] = activeTBlit;
                CKmasterBuffer.virtualBg[vY][vX]   = activeBBlit;
            }
        }
        win->cursorPos.x++; 
        bytesCommitted++;
    }

    win->isDirty = true; 
    CK_ENV.termPending = true;
    return bytesCommitted;
}

/* STAGE 2: THE COMPOSITOR VIEW TREE COUPLER */
// 1. Non-Recursive View Matrix Compositor Node Visitor
static bool CompositeNodeVisitor(CKgraphNodeHandle node, void* context) {
    if (!node) return false;

    // Unpack the type-agnostic node handle straight to our private structural layout
    CKwindowNode* win = (CKwindowNode*)node;
    
    // Core Feature Requirement: Even if a window is hidden or clean, we still process it 
    // to clear out stale artifacts or route transparent blanks down if it's invisible.
    int width  = win->windowSize.x;
    int height = win->windowSize.y;

    for (int relativeY = 0; relativeY < height; relativeY++) {
        for (int relativeX = 0; relativeX < width; relativeX++) {
            
            int physicalX = win->windowPos.startX + relativeX;
            int physicalY = win->windowPos.startY + relativeY;

            // Enforce strict physical hardware viewport monitor bounds
            if (physicalX >= 0 && physicalX < CK_COLS && physicalY >= 0 && physicalY < CK_ROWS) {
                int virtualX = win->windowBufferPos.startX + relativeX;
                int virtualY = win->windowBufferPos.startY + relativeY;

                char virtualBgToken, virtualTxtToken, virtualChar;

                // ARCHITECTURAL UPGRADE INTERCEPT: Non-Destructive Visibility Masking
                // If the window container's hidden bit is flag-set, evaluate alpha pass-through tokens.
                // This shields lower display contexts from drawing corruption without altering virtual memory tracks!
                if (!win->isVisible) {
                    virtualBgToken  = BLIT_TRANSPARENT_CHAR; // 'g'
                    virtualTxtToken = BLIT_TRANSPARENT_CHAR; // 'g'
                    virtualChar     = CK_GLYPH_VOID;         // 0xFF
                } else {
                    // Standard visibility: read content directly from the window's virtual partition slice
                    virtualBgToken  = CKmasterBuffer.virtualBg[virtualY][virtualX];
                    virtualTxtToken = CKmasterBuffer.virtualText[virtualY][virtualX];
                    virtualChar     = CKmasterBuffer.virtualChar[virtualY][virtualX];
                }

                // --- LAYER COMPOSITING PIPELINE ---
                // Channel 1: Background Color Layer Tracker
                if (virtualBgToken != BLIT_TRANSPARENT_CHAR) {
                    CKmasterBuffer.physicalBg[physicalY][physicalX] = virtualBgToken;
                }
                
                // Channel 2: Foreground Color Layer Tracker 
                if (virtualTxtToken != BLIT_TRANSPARENT_CHAR) {
                    CKmasterBuffer.physicalText[physicalY][physicalX] = virtualTxtToken;
                }

                // Channel 3: Glyph Shape Character String Tracker
                if (virtualChar != CK_GLYPH_VOID) {
                    CKmasterBuffer.physicalChar[physicalY][physicalX] = virtualChar;
                }
            }
        }
    }

    // ROUND-UP PHASE: Cleanse the window's local dirty state tracking register
    win->isDirty = false;

    return true; // Return true to advance to the next Z-index sibling node smoothly
}

// 2. The Upgraded, Non-Recursive View Tree Coupler Interface
void CKterminal_CompositeTree(CKwindowNode* root) {
    if (root == NULL) return;

    // Trigger our iterative, non-recursive pre-order layout traversal pass.
    // The link arrangement order of siblings naturally maps the correct back-to-front rendering Z-indices!
    CKgraph_Flatten((CKgraphNodeHandle)root, CK_PRE_ORDER, CompositeNodeVisitor, NULL);
}

bool CKclearContext_int(CKwindowNode* node, char tBlit, char bBlit, int line) {
    if (node == NULL) return false;

    // Determine vertical bounding limits based on optional parameter state
    int startY = 0;
    int endY = node->windowSize.y;

    if (line != -1) {
        // Enforce localized clip constraints if a single relative line is targeted
        if (line < 0 || line >= node->windowSize.y) return false;
        startY = line;
        endY = line + 1;
    }

    // ARCHITECTURAL UPGRADE: If the background blit targets alpha pass-through ('g'),
    // we use CK_GLYPH_VOID so lower characters show through. Otherwise, we stamp spaces (' ').
    char fillGlyph = (bBlit == BLIT_TRANSPARENT_CHAR) ? CK_GLYPH_VOID : ' ';

    // Traverse targets inside this window node's virtual coordinate block
    for (int y = startY; y < endY; y++) {
        for (int x = 0; x < node->windowSize.x; x++) {
            int vX = node->windowBufferPos.startX + x;
            int vY = node->windowBufferPos.startY + y;

            if (vX >= 0 && vX < CK_MAX_CANVAS_COLS && vY >= 0 && vY < CK_MAX_CANVAS_ROWS) {
                CKmasterBuffer.virtualChar[vY][vX] = fillGlyph;
                CKmasterBuffer.virtualText[vY][vX] = tBlit;
                CKmasterBuffer.virtualBg[vY][vX]   = bBlit;
            }
        }
    }

    // Update state flags
    if (line == -1) {
        node->cursorPos.x = 0;
        node->cursorPos.y = 0;
    } else {
        node->cursorPos.x = 0; // ClearLine loops only reset horizontal spacing
    }
    
    // Modernized Intercept: Use our unified cascade flag system instead of a manual assignment 
    CKterminal_CascadeFlag((CKwindowHandle)node, CK_CASCADE_DIRTY);
    
    CK_ENV.termPending = true;
    return true;
}

void CKterminal_DumpState(const char* filename) {
    if (filename == NULL) return;

    FILE* dumpFile = NULL;
    // Using standard C11 secure file locks
    if (fopen_s(&dumpFile, filename, "w") != 0 || dumpFile == NULL) {
        return;
    }

    fprintf(dumpFile, "========================================================================\n");
    fprintf(dumpFile, "       CHARIKLO OS SYSTEM ARCHITECTURE MATRIX DISCOVERY HARNESS\n");
    fprintf(dumpFile, "========================================================================\n\n");

    // 1. DUMP THE FLAT PHYSICAL COMPOSITOR LAYER (What Stage 3 physically paints)
    fprintf(dumpFile, "--- [STAGE 2 VIEW SURFACE: PHYSICAL MATRIX SECTOR (%dx%d)] ---\n", CK_ROWS, CK_COLS);
    
    fprintf(dumpFile, "\n[PHYSICAL CHARACTER GRID]\n");
    for (int y = 0; y < CK_ROWS; y++) {
        fprintf(dumpFile, "Row %02d: |", y);
        for (int x = 0; x < CK_COLS; x++) {
            fputc(CKmasterBuffer.physicalChar[y][x], dumpFile);
        }
        fprintf(dumpFile, "|\n");
    }

    fprintf(dumpFile, "\n[PHYSICAL TEXT BLIT TOKENS]\n");
    for (int y = 0; y < CK_ROWS; y++) {
        fprintf(dumpFile, "Row %02d: |", y);
        for (int x = 0; x < CK_COLS; x++) {
            fputc(CKmasterBuffer.physicalText[y][x], dumpFile);
        }
        fprintf(dumpFile, "|\n");
    }

    fprintf(dumpFile, "\n[PHYSICAL BACKGROUND BLIT TOKENS]\n");
    for (int y = 0; y < CK_ROWS; y++) {
        fprintf(dumpFile, "Row %02d: |", y);
        for (int x = 0; x < CK_COLS; x++) {
            fputc(CKmasterBuffer.physicalBg[y][x], dumpFile);
        }
        fprintf(dumpFile, "|\n");
    }

    // 2. DUMP THE SECTOR VIRTUAL PARTITIONS (Raw data tracks where Stage 1 commits)
    fprintf(dumpFile, "\n\n========================================================================\n");
    fprintf(dumpFile, "--- [STAGE 1 BUFFER TRACKS: VIRTUAL MATRIX SECTOR (%dx%d)] ---\n", CK_MAX_CANVAS_ROWS, CK_MAX_CANVAS_COLS);
    fprintf(dumpFile, "========================================================================\n");

    fprintf(dumpFile, "\n[VIRTUAL CHARACTER ACQUISITION PLANES]\n");
    for (int y = 0; y < CK_MAX_CANVAS_ROWS; y++) {
        // Optimization: Only print virtual tracks that are not totally unallocated spaces
        bool rowHasData = false;
        for (int x = 0; x < CK_MAX_CANVAS_COLS; x++) {
            if (CKmasterBuffer.virtualChar[y][x] != ' ' || CKmasterBuffer.virtualBg[y][x] != BLIT_TRANSPARENT_CHAR) {
                rowHasData = true;
                break;
            }
        }
        if (!rowHasData) continue; // Skip large dead memory zones to keep text file small

        fprintf(dumpFile, "V-Row %03d: |", y);
        for (int x = 0; x < CK_MAX_CANVAS_COLS; x++) {
            char c = CKmasterBuffer.virtualChar[y][x];
            
            // Visual Anchor for debugging void glyph
            if (c == CK_GLYPH_VOID) {
                fputc('.', dumpFile);
            } else {
                fputc(c, dumpFile);
            }
        }
        fprintf(dumpFile, "|\n");
    }

    fprintf(dumpFile, "\n[VIRTUAL BACKGROUND BLIT PLANES]\n");
    for (int y = 0; y < CK_MAX_CANVAS_ROWS; y++) {
        bool rowHasData = false;
        for (int x = 0; x < CK_MAX_CANVAS_COLS; x++) {
            if (CKmasterBuffer.virtualBg[y][x] != BLIT_TRANSPARENT_CHAR) {
                rowHasData = true;
                break;
            }
        }
        if (!rowHasData) continue;

        fprintf(dumpFile, "V-Row %03d: |", y);
        for (int x = 0; x < CK_MAX_CANVAS_COLS; x++) {
            fputc(CKmasterBuffer.virtualBg[y][x], dumpFile);
        }
        fprintf(dumpFile, "|\n");
    }

    fclose(dumpFile);
}
