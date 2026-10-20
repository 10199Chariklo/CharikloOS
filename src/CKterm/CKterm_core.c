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


void CKsetCursorPos(const int x, const int y) {
    CKwindowNode* win = g_current;
    if (win == NULL) return;

    // CraftOS uses 1-indexed console spacing layout positions. 
    // We translate down to standard C 0-indexed layout bounds.
    win->cursorPos.x = x - 1;
    win->cursorPos.y = y - 1;
    CK_ENV.termPending = true;
}

void CKsetCursorBlink(const bool blink) {
    CKwindowNode* win = g_current;
    if (win == NULL) return;

    win->cursorBlink = blink;
    CK_ENV.termPending = true;
}

void CKgetCursorPos(int *x, int *y) {
    CKwindowNode* win = g_current;
    if (win == NULL) { *x = 1; *y = 1; return; }
    
    // Translate internal 0-indexed bounds back to public 1-indexed metrics
    *x = win->cursorPos.x + 1;
    *y = win->cursorPos.y + 1;
}

bool CKgetCursorBlink(bool *state) {
    CKwindowNode* win = g_current;
    if (win == NULL) { *state = false; return false; }
    *state = win->cursorBlink;
    return true;
}

void CKgetSize(int *x, int *y) {
    CKwindowNode* win = g_current;
    if (win == NULL) { *x = CK_COLS; *y = CK_ROWS; return; }
    *x = win->windowSize.x;
    *y = win->windowSize.y;
}

bool CKclear(void) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;

     // Default to transparent
    char activeTBlit = 'g';
    char activeBBlit = 'g';

    // If the node is a parent we check it's stateful colors
    if (!win->parent) {
        // Lookup active tokens character matching configuration definitions
        for (int i = 0; i < 16; i++) {
            if (CKpalette.data[i].colorID == win->activeBackgroundColor) {
                activeBBlit = CKpalette.data[i].blitChar;
                break;
            }
            if (CKpalette.data[i].colorID == win->activeTextColor) {
                activeTBlit = CKpalette.data[i].blitChar;
                break;
            }
        }
    }

    // Isolate deletions strictly inside this window node's virtual coordinate block
    for (int y = 0; y < win->windowSize.y; y++) {
        for (int x = 0; x < win->windowSize.x; x++) {
            int vX = win->windowBufferPos.startX + x;
            int vY = win->windowBufferPos.startY + y;

            if (vX >= 0 && vX < CK_MAX_CANVAS_COLS && vY >= 0 && vY < CK_MAX_CANVAS_ROWS) {
                CKmasterBuffer.virtualChar[vY][vX] = ' ';
                CKmasterBuffer.virtualText[vY][vX] = activeTBlit;
                CKmasterBuffer.virtualBg[vY][vX]   = activeBBlit;
            }
        }
    }

    win->cursorPos.x = 0;
    win->cursorPos.y = 0;
    win->isDirty = true;
    CK_ENV.termPending = true;
    return true;
}

bool CKclearLine(void) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;

    int y = win->cursorPos.y;
    if (y < 0 || y >= win->windowSize.y) return false;

    char activeBBlit = 'f';
    for (int i = 0; i < 16; i++) {
        if (CKpalette.data[i].colorID == win->activeBackgroundColor) {
            activeBBlit = CKpalette.data[i].blitChar;
            break;
        }
    }

    for (int x = 0; x < win->windowSize.x; x++) {
        int vX = win->windowBufferPos.startX + x;
        int vY = win->windowBufferPos.startY + y;

        if (vX >= 0 && vX < CK_MAX_CANVAS_COLS && vY >= 0 && vY < CK_MAX_CANVAS_ROWS) {
            CKmasterBuffer.virtualChar[vY][vX] = ' ';
            CKmasterBuffer.virtualText[vY][vX] = '0';
            CKmasterBuffer.virtualBg[vY][vX]   = activeBBlit;
        }
    }

    win->cursorPos.x = 0;
    win->isDirty = true;
    CK_ENV.termPending = true;
    return true;
}

unsigned int CKgetTextColor(void) {
    CKwindowNode* win = g_current;
    return win ? win->activeTextColor : 0x0001;
}

unsigned int CKgetBackgroundColor(void) {
    CKwindowNode* win = g_current;
    return win ? win->activeBackgroundColor : 0x8000;
}

bool CKsetTextColor(unsigned int colorsMask) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;
    win->activeTextColor = colorsMask;
    return true;
}

bool CKsetBackgroundColor(unsigned int colorsMask) {
    CKwindowNode* win = g_current;
    if (win == NULL) return false;
    win->activeBackgroundColor = colorsMask;
    return true;
}
