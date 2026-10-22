/* Standard C11 Libraries */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* Chariklo Subsystem */
#include "OUterm.h" // Local module header
#include "CKtypes_int.h"
#include "CKterminal_int.h"
#include "CKpalette_int.h"
#include "OUcolors.h"
#include "OUwindow.h"

int CKprint(const char* format, ...) {
    if (format == NULL) return 0;

    va_list args;
    va_start(args, format);
    
    char scratchPad[1024];
    int length = vsnprintf(scratchPad, sizeof(scratchPad), format, args);
    va_end(args);

    if (length > 0) {
        CKwrite(scratchPad); 
    }
    return length;
}

void CKwrite(const char* text) {
    if (text == NULL) return;
    CKwindowNode* win = g_current;
    if (win == NULL || !win->isVisible) return;

    // Use our standardized Stage 1 layout primitive engine to handle data layout strings
    CKfwritef(win, text, strlen(text));
}

void CKblit(const char* text, const char* textColors, const char* bgColors) {
    if (text == NULL || textColors == NULL || bgColors == NULL) return;
    CKwindowNode* win = g_current;
    if (win == NULL || !win->isVisible) return;

    int textLen = (int)strlen(text);
    if (textLen != (int)strlen(textColors) || textLen != (int)strlen(bgColors)) return;

    int y = win->cursorPos.y;
    if (y < 0 || y >= win->windowSize.y) return;

    // Direct Virtual Memory Buffer Write Pass
    for (int i = 0; i < textLen; i++) {
        int targetX = win->cursorPos.x + i;
        
        // Enforce localized bounding box clip constraints
        if (targetX >= 0 && targetX < win->windowSize.x) {
            int vX = win->windowBufferPos.startX + targetX;
            int vY = win->windowBufferPos.startY + y;

            if (vX >= 0 && vX < CK_MAX_CANVAS_COLS && vY >= 0 && vY < CK_MAX_CANVAS_ROWS) {
                CKmasterBuffer.virtualChar[vY][vX] = text[i];
                CKmasterBuffer.virtualText[vY][vX] = textColors[i];
                CKmasterBuffer.virtualBg[vY][vX]   = bgColors[i];
            }
        }
    }

    win->cursorPos.x += textLen;
    
    // ARCHITECTURAL CORRECTION: Bypassing a local window dirty flag is insufficient for transparency.
    // Because transparent windows rely on rendering data from windows underneath them, a change to an
    // overlay requires a full topology refresh to force historical caches to re-evaluate structural layers.
    if (CK_ENV.nativeWindowNode != NULL) {
        CKterminal_CascadeDirty(CK_ENV.nativeWindowNode);
    }
    
    CK_ENV.termPending = true;
}

/** */
void CKgetCursorPos(int *x, int *y) {
    CKwindowNode* win = g_current;
    if (win == NULL) { *x = 1; *y = 1; return; }
    
    // Translate internal 0-indexed bounds back to public 1-indexed metrics
    *x = win->cursorPos.x + 1;
    *y = win->cursorPos.y + 1;
}
/** */
void CKsetCursorPos(const int x, const int y) {
    CKwindowNode* win = g_current;
    if (win == NULL) return;

    // CraftOS uses 1-indexed console spacing layout positions. 
    // We translate down to standard C 0-indexed layout bounds.
    win->cursorPos.x = x - 1;
    win->cursorPos.y = y - 1;
    CK_ENV.termPending = true;
}

/** */
bool CKgetCursorBlink(bool *state) {
    CKwindowNode* win = g_current;
    if (win == NULL) { *state = false; return false; }
    *state = win->cursorBlink;
    return true;
}
/** */
void CKsetCursorBlink(const bool blink) {
    CKwindowNode* win = g_current;
    if (win == NULL) return;

    win->cursorBlink = blink;
    CK_ENV.termPending = true;
}

void CKgetSize(int *x, int *y) {
    CKwindowNode* win = g_current;
    if (win == NULL) { *x = CK_COLS; *y = CK_ROWS; return; }
    *x = win->windowSize.x;
    *y = win->windowSize.y;
}

/** */
bool CKfill(void) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;

    // Default to white fg / black bg blit fallbacks
    char activeTBlit = '0';
    char activeBBlit = 'f';

    // Lookup active tokens character matching configurations
    for (int i = 0; i < 16; i++) {
        if (CKpalette.data[i].colorID == win->activeBackgroundColor) {
            activeBBlit = CKpalette.data[i].blitChar;
        }
        if (CKpalette.data[i].colorID == win->activeTextColor) {
            activeTBlit = CKpalette.data[i].blitChar;
        }
    }

    // Leverages our new primitive, leaving line blank (Defaults to full segment wipe)
    return CKclearContext(win, activeTBlit, activeBBlit);
}
/** */
bool CKfillLine(void) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;

    // Isolate the row operation to the active vertical tracking bounds
    int currentLine = win->cursorPos.y;
    if (currentLine < 0 || currentLine >= win->windowSize.y) return false;

    // Default palette lookup fallbacks
    char activeTBlit = '0'; // White
    char activeBBlit = 'f'; // Black

    // Map your custom parent-inherited bitmasks back to raw blit text tokens
    for (int i = 0; i < 16; i++) {
        if (CKpalette.data[i].colorID == win->activeBackgroundColor) {
            activeBBlit = CKpalette.data[i].blitChar;
        }
        if (CKpalette.data[i].colorID == win->activeTextColor) {
            activeTBlit = CKpalette.data[i].blitChar;
        }
    }

    // Leverages our central context engine by passing an explicit target row index constraint.
    // Because activeBBlit is an opaque palette character, CKclearContext automatically 
    // applies true empty text characters (' ') instead of CK_GLYPH_VOID.
    return CKclearContext(win, activeTBlit, activeBBlit, currentLine);
}

/** */
bool CKclear(void) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;

    // If the node is a primary top-level root, default to a complete palette fill
    if (win->parent == NULL) {
        return CKfill();
    }

    // Transparent windows wipe clear back down to their alpha pass-through layers
    return CKclearContext(win, BLIT_TRANSPARENT_CHAR, BLIT_TRANSPARENT_CHAR);
}
/** */
bool CKclearLine(void) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;

    // If the node is a primary top-level root, default to a complete palette fill
    if (win->parent == NULL) {
        return CKfillLine();
    }

    int currentLine = win->cursorPos.y;
    if (currentLine < 0 || currentLine >= win->windowSize.y) return false;

    // Leverages our primitive by passing an explicit target row index constraint
    return CKclearContext(win, BLIT_TRANSPARENT_CHAR, BLIT_TRANSPARENT_CHAR, currentLine);
}

/** */
unsigned int CKgetTextColor(void) {
    CKwindowNode* win = g_current;
    return win ? win->activeTextColor : 0x0001;
}
/** */
bool CKsetTextColor(unsigned int colorsMask) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;
    win->activeTextColor = colorsMask;
    return true;
}

/** */
unsigned int CKgetBackgroundColor(void) {
    CKwindowNode* win = g_current;
    return win ? win->activeBackgroundColor : 0x8000;
}
/** */
bool CKsetBackgroundColor(unsigned int colorsMask) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;
    win->activeBackgroundColor = colorsMask;
    return true;
}
