#if defined(_WIN32) || defined(_WIN32_)
#include <windows.h>

/* Standard C11 Libraries */
#include <stdbool.h>
#include <stdio.h>

/* Chariklo Subsystem */
#include "CKterminal_int.h" 
#include "CKpalette_int.h"

// Keep a clean thread-safe backup of the previous frame state to prevent terminal flickering
static char historicalCharBuffer[CK_ROWS][CK_COLS] = {0};
static char historicalTextBlit[CK_ROWS][CK_COLS] = {0};
static char historicalBgBlit[CK_ROWS][CK_COLS] = {0};

/* STAGE 3: THE PHYSICAL HARDWARE COMPOSITOR INTERFACES */
void CKplatformRefresh(void) {
    if (!CK_ENV.termPending) return;

    // STEP 1: Flatten sibling tree nodes sequentially down onto the flat physical matrices
    if (CK_ENV.nativeWindowNode != NULL) {
        CKterminal_CompositeTree(CK_ENV.nativeWindowNode);
    }

    // STEP 2: Force home position on our console viewport space
    printf("\x1b[H");

    char fgSequence[64] = {0};
    char bgSequence[64] = {0};
    char lastTxtChar = '\0';
    char lastFgBlit = '\0';
    char lastBgBlit = '\0';

    for (int y = 0; y < CK_ROWS; y++) {
        for (int x = 0; x < CK_COLS; x++) {
            
            char currentRawChar  = CKmasterBuffer.physicalChar[y][x];
            char currentTextBlit = CKmasterBuffer.physicalText[y][x];
            char currentBgBlit   = CKmasterBuffer.physicalBg[y][x];

            // Double-Diff Optimization Check compiles cleanly against pre-composited screens
            if (currentRawChar  != historicalCharBuffer[y][x] || 
                currentTextBlit != historicalTextBlit[y][x]   || 
                currentBgBlit   != historicalBgBlit[y][x]) {

                printf("\x1b[%d;%dH", y + 1, x + 1);

                // --- BITMASK TRACKING REGISTER ---
                // Bit 0 (0x1): Foreground color changed
                // Bit 1 (0x2): Background color changed
                int syncContextState = 0;
                if (currentTextBlit != lastFgBlit) syncContextState |= 0x1;
                if (currentBgBlit   != lastBgBlit) syncContextState |= 0x2;

                // Execute a clean, linear state execution pass based on the tracked changes
                switch (syncContextState) {
                    
                    case 0x1: /* ONLY Foreground Changed */
                        if (CKwin_GetCSISequence(fgSequence, sizeof(fgSequence), currentTextBlit, CK_COLOR_TARGET_FOREGROUND, x, y)) {
                            printf("%s", fgSequence);
                            lastFgBlit = currentTextBlit;
                        }
                        // HOST SAFETY FORCE: Re-emit active background color to maintain terminal synchronization
                        if (CKwin_GetCSISequence(bgSequence, sizeof(bgSequence), lastBgBlit, CK_COLOR_TARGET_BACKGROUND, x, y)) {
                            printf("%s", bgSequence);
                        }
                        break;

                    case 0x2: /* ONLY Background Changed */
                        // HOST SAFETY FORCE: Re-emit active foreground color to maintain terminal synchronization
                        if (CKwin_GetCSISequence(fgSequence, sizeof(fgSequence), lastFgBlit, CK_COLOR_TARGET_FOREGROUND, x, y)) {
                            printf("%s", fgSequence);
                        }
                        if (CKwin_GetCSISequence(bgSequence, sizeof(bgSequence), currentBgBlit, CK_COLOR_TARGET_BACKGROUND, x, y)) {
                            printf("%s", bgSequence);
                            lastBgBlit = currentBgBlit;
                        }
                        break;

                    case 0x3: /* BOTH Channels Changed */
                        if (CKwin_GetCSISequence(fgSequence, sizeof(fgSequence), currentTextBlit, CK_COLOR_TARGET_FOREGROUND, x, y)) {
                            printf("%s", fgSequence);
                            lastFgBlit = currentTextBlit;
                        }
                        if (CKwin_GetCSISequence(bgSequence, sizeof(bgSequence), currentBgBlit, CK_COLOR_TARGET_BACKGROUND, x, y)) {
                            printf("%s", bgSequence);
                            lastBgBlit = currentBgBlit;
                        }
                        break;

                    case 0x0: /* Neither color changed (Only character string update occurred) */
                    default:
                        break;
                }

                putchar(currentRawChar);
                lastTxtChar = currentRawChar;

                // Synchronize the localized double-diff tracking array boxes
                historicalCharBuffer[y][x] = currentRawChar;
                historicalTextBlit[y][x]   = currentTextBlit;
                historicalBgBlit[y][x]     = currentBgBlit;
            }
        }
    }

    // Resolve hardware cursor parameters cleanly
    CKwindowNode* activeWin = g_current;
    if (activeWin != NULL) {
        int absoluteCursorX = activeWin->windowPos.startX + activeWin->cursorPos.x;
        int absoluteCursorY = activeWin->windowPos.startY + activeWin->cursorPos.y;

        if (absoluteCursorX >= 0 && absoluteCursorX < CK_COLS &&
            absoluteCursorY >= 0 && absoluteCursorY < CK_ROWS) {
            
            printf("\x1b[%d;%dH", absoluteCursorY + 1, absoluteCursorX + 1);
            printf(activeWin->cursorBlink ? "\x1b[?25h" : "\x1b[?25l");
        }
    }

    fflush(stdout);
    CK_ENV.termPending = false;
}

#endif /* _WIN32 */