/* Standard C11 Libraries */
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

/* Chariklo Subsystem */
    /* Local module headers */
#include "OUterm.h"
    /* Distant module headers */
//#include "CKio.h"
#include "CKtypes.h"
#include "OUcolors.h"
#include "CKwindow.h"
#include "CKterminal_int.h"
#include "CKpalette_int.h"

int CKprint(const char* format, ...) {
    if (format == NULL) return 0;

    va_list args;
    va_start(args, format);
    
    // Systems Integration Pass: We bypass standard library printing entirely.
    // Instead, we bounce the formatting arguments directly up into Chariklo's 
    // context-redirected CKprintf pipeline, which routes out to the active window.
    char scratchPad[1024];
    int length = vsnprintf(scratchPad, sizeof(scratchPad), format, args);
    va_end(args);

    if (length > 0) {
        CKwrite(scratchPad); // Safely pass down through our bounding box clip gates
    }
    return length;
}

void CKwrite(const char* text) {
    if (text == NULL) return;
    CKwindowNode* win = g_current;
    if (win == NULL || !win->isVisible) return;

    int length = (int)strlen(text);
    int y = win->cursorPos.y;

    // Local boundary visibility safety check
    if (y < 0 || y >= win->windowSize.y) return;


    // Map active color identities to blit tokens
    char activeTBlit = '0';
    char activeBBlit = 'f';
    for (int i = 0; i < 16; i++) {
        if (CKcolorsInt.data[i].colorID == win->activeTextColor) {
            activeTBlit = CKcolorsInt.data[i].blitChar;
        }
        if (CKcolorsInt.data[i].colorID == win->activeBackgroundColor) {
            activeBBlit = CKcolorsInt.data[i].blitChar;
        }
    }

    // Build temporary character blit chains to recycle logic safely via structural commands
    char defaultTextBlit[256] = {0};
    char defaultBgBlit[256] = {0};
    
    int fitLen = (length < 255) ? length : 255;
    memset(defaultTextBlit, activeTBlit, fitLen);
    memset(defaultBgBlit, activeBBlit, fitLen);

    CKblit(text, defaultTextBlit, defaultBgBlit);
}

void CKblit(const char* text, const char* textColors, const char* bgColors) {
    if (text == NULL || textColors == NULL || bgColors == NULL) return;
    CKwindowNode* win = g_current;
    if (win == NULL || !win->isVisible) return;

    int textLen = (int)strlen(text);
    if (textLen != (int)strlen(textColors) || textLen != (int)strlen(bgColors)) return;

    int y = win->cursorPos.y;
    if (y < 0 || y >= win->windowSize.y) return;

    // Calculate absolute positions relative to the scene tree root window layout
    int absoluteY = win->windowPos.startY + y;
    if (absoluteY < 0 || absoluteY >= CK_ROWS) return;

    for (int i = 0; i < textLen; i++) {
        int targetX = win->cursorPos.x + i;
        if (targetX >= 0 && targetX < win->windowSize.x) {
            int absoluteX = win->windowPos.startX + targetX;
            if (absoluteX >= 0 && absoluteX < CK_COLS) {
                CKmasterBuffer.charBuffer[absoluteY][absoluteX] = text[i];
                CKmasterBuffer.textBlitBuffer[absoluteY][absoluteX] = textColors[i];
                CKmasterBuffer.bgBlitBuffer[absoluteY][absoluteX] = bgColors[i];
            }
        }
    }

    win->cursorPos.x += textLen;
    win->isDirty = true;
    CK_ENV.termPending = true;
}

void CKsetCursorPos(const int x, const int y) {
    CKwindowNode* win = g_current;
    if (win == NULL) return;

    win->cursorPos.x = x;
    win->cursorPos.y = y;
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
    if (win == NULL) { *x = 0; *y = 0; return; }
    *x = win->cursorPos.x;
    *y = win->cursorPos.y;
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

    char activeBBlit = 'f';
    for (int i = 0; i < 16; i++) {
        if (CKcolorsInt.data[i].colorID == win->activeBackgroundColor) {
            activeBBlit = CKcolorsInt.data[i].blitChar;
            break;
        }
    }

    // Clear space restricted within our active window bounds layout
    for (int y = 0; y < win->windowSize.y; y++) {
        int absoluteY = win->windowPos.startY + y;
        if (absoluteY < 0 || absoluteY >= CK_ROWS) continue;

        for (int x = 0; x < win->windowSize.x; x++) {
            int absoluteX = win->windowPos.startX + x;
            if (absoluteX < 0 || absoluteX >= CK_COLS) continue;

            CKmasterBuffer.charBuffer[absoluteY][absoluteX] = ' ';
            CKmasterBuffer.textBlitBuffer[absoluteY][absoluteX] = '0';
            CKmasterBuffer.bgBlitBuffer[absoluteY][absoluteX] = activeBBlit;
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

    int absoluteY = win->windowPos.startY + y;
    if (absoluteY < 0 || absoluteY >= CK_ROWS) return false;

    char activeBBlit = 'f';
    for (int i = 0; i < 16; i++) {
        if (CKcolorsInt.data[i].colorID == win->activeBackgroundColor) {
            activeBBlit = CKcolorsInt.data[i].blitChar;
            break;
        }
    }

    for (int x = 0; x < win->windowSize.x; x++) {
        int absoluteX = win->windowPos.startX + x;
        if (absoluteX < 0 || absoluteX >= CK_COLS) continue;

        CKmasterBuffer.charBuffer[absoluteY][absoluteX] = ' ';
        CKmasterBuffer.textBlitBuffer[absoluteY][absoluteX] = '0';
        CKmasterBuffer.bgBlitBuffer[absoluteY][absoluteX] = activeBBlit;
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