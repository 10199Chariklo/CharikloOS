/* Chariklo Subsystem */
    /* Local module headers */
#include "CKterminal_int.h"
    /* Distant module headers */
#include "CKpalette_int.h"

// Define the global master engine buffer instance
CKmasterTerminalBuffer CKmasterBuffer;

// Concrete initialization instance of the global Chariklo execution environment registers
CKenv CK_ENV = {
    .nativeWindowNode = NULL,
    .currentWindowNode = NULL,
    .termPending = false,
    .activeSystemTick = 0
};

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
    CKmasterBuffer.viewportX = 0;
    CKmasterBuffer.viewportY = 0;
    CKmasterBuffer.viewportWidth = CK_COLS;
    CKmasterBuffer.viewportHeight = CK_ROWS;

    // Flood clear the single master canvas to raw defaults in one rapid sweep
    memset(CKmasterBuffer.charBuffer, ' ', sizeof(CKmasterBuffer.charBuffer));
    memset(CKmasterBuffer.textBlitBuffer, '0', sizeof(CKmasterBuffer.textBlitBuffer));
    memset(CKmasterBuffer.bgBlitBuffer, BLIT_TRANSPARENT_CHAR, sizeof(CKmasterBuffer.bgBlitBuffer));
}

CKwindowNode* CKwindow_Create(int x, int y, int width, int height) {
    CKwindowNode* node = (CKwindowNode*)malloc(sizeof(CKwindowNode));
    if (node == NULL) return NULL;

    // Calculate absolute screen-space bounds immediately at construction
    node->windowPos.startX = x;
    node->windowPos.startY = y;
    node->windowPos.endX   = x + width;
    node->windowPos.endY   = y + height;

    // Retain explicit local size pairings for localized relative calculations
    node->windowSize.x = width;
    node->windowSize.y = height;

    // Home localized cursor vectors
    node->cursorPos.x = 0;
    node->cursorPos.y = 0;

    node->cursorBlink = true;
    node->activeTextColor = 0x0001;       // White
    node->activeBackgroundColor = 0x8000; // Black
    node->isDirty = true;
    node->isVisible = true;
    
    node->parent = NULL;
    node->firstChild = NULL;
    node->prevSibling = NULL;
    node->nextSibling = NULL;

    return node;
}

void CKterminal_CascadeDirty(CKwindowNode* root) {
    if (root == NULL) return;
    root->isDirty = true;

    CKwindowNode* child = root->firstChild;
    while (child != NULL) {
        CKterminal_CascadeDirty(child);
        child = child->nextSibling;
    }
}

void CKterminal_FreeWindowNode(CKwindowNode* node) {
    if (node == NULL) return;

    // Safely detach hierarchical topological nodes before executing memory cleanups
    if (node->prevSibling) node->prevSibling->nextSibling = node->nextSibling;
    if (node->nextSibling) node->nextSibling->prevSibling = node->prevSibling;
    if (node->parent && node->parent->firstChild == node) node->parent->firstChild = node->nextSibling;

    // CKTBO and CKTBOex allocations are both safely released through the common base header
    free(node);
    printf("[CKgc] Target Window Object successfully collected and freed.\n");
}

int CKfwritef(CKwindowNode* win, const void* buffer, size_t count) {
    if (win == NULL || buffer == NULL || count == 0 || !win->isVisible) return 0;
    
    const char* sourceBytes = (const char*)buffer;
    int bytesCommitted = 0;

    // Cache blit mappings to optimize lookup execution overhead loops
    char activeTBlit = '0'; char activeBBlit = 'f';
    for (int i = 0; i < 16; i++) {
        if (CKcolorsInt.data[i].colorID == win->activeTextColor)       activeTBlit = CKcolorsInt.data[i].blitChar;
        if (CKcolorsInt.data[i].colorID == win->activeBackgroundColor) activeBBlit = CKcolorsInt.data[i].blitChar;
    }

    for (size_t i = 0; i < count; i++) {
        char character = sourceBytes[i];

        if (character == '\n') {
            win->cursorPos.x = 0;
            win->cursorPos.y++;
            bytesCommitted++;
            continue;
        }

        // 1. Safe Relative Local Metric Clip Check
        if (win->cursorPos.x >= 0 && win->cursorPos.x < win->windowSize.x &&
            win->cursorPos.y >= 0 && win->cursorPos.y < win->windowSize.y) {

            // 2. Map directly using the pre-calculated start vector tracking registers
            int absoluteX = win->windowPos.startX + win->cursorPos.x;
            int absoluteY = win->windowPos.startY + win->cursorPos.y;

            // 3. Absolute Platform Screen Boundary Check
            if (absoluteX >= 0 && absoluteX < CK_COLS && 
                absoluteY >= 0 && absoluteY < CK_ROWS) {

                if (activeBBlit != BLIT_TRANSPARENT_CHAR) {
                    CKmasterBuffer.bgBlitBuffer[absoluteY][absoluteX] = activeBBlit;
                }

                CKmasterBuffer.charBuffer[absoluteY][absoluteX] = character;
                CKmasterBuffer.textBlitBuffer[absoluteY][absoluteX] = activeTBlit;
            }
        }
        win->cursorPos.x++; 
        bytesCommitted++;
    }

    win->isDirty = true; 
    CK_ENV.termPending = true;
    return bytesCommitted;
}