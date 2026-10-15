#if defined(_WIN32) || defined(_WIN32_)

/* Chariklo Subsystem */
    /* Local module headers */
#include "CKterminal_int.h"
    /* Distant module headers */
#include "CKpalette_int.h"

// Keep a clean thread-safe backup of the previous frame state to prevent terminal flickering
static char historicalCharBuffer[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS] = {0};
static char historicalTextBlit[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS] = {0};
static char historicalBgBlit[CK_MAX_CANVAS_ROWS][CK_MAX_CANVAS_COLS] = {0};

void CKplatformRefresh(void) {
    if (!CK_ENV.termPending) return;

    // Optimization check: Use standard stdout streaming for high-speed ANSI escape rendering
    // 1. Force home position on our console viewport space
    printf("\x1b[H");

    char fgSequence[64] = {0};
    char bgSequence[64] = {0};
    char lastFgBlit = '\0';
    char lastBgBlit = '\0';

    for (int y = 0; y < CK_ROWS; y++) {
        for (int x = 0; x < CK_COLS; x++) {
            char currentRawChar = CKmasterBuffer.charBuffer[y][x];
            char currentTextBlit = CKmasterBuffer.textBlitBuffer[y][x];
            char currentBgBlit   = CKmasterBuffer.bgBlitBuffer[y][x];

            // Only push text changes downstream if cell updates vary from historical frame steps
            if (currentRawChar  != historicalCharBuffer[y][x] || 
                currentTextBlit != historicalTextBlit[y][x]   || 
                currentBgBlit   != historicalBgBlit[y][x]) {

                // Jump cursor directly to targeted coordinates inside your ANSI frame layout
                printf("\x1b[%d;%dH", y + 1, x + 1);

                // Dynamically look up and switch foreground text streams
                if (currentTextBlit != lastFgBlit) {
                    if (CKwin_GetCSISequence(fgSequence, sizeof(fgSequence), currentTextBlit, CK_COLOR_TARGET_FOREGROUND, x, y)) {
                        printf("%s", fgSequence);
                        lastFgBlit = currentTextBlit;
                    }
                }

                // Dynamically look up and switch background paint layers
                if (currentBgBlit != lastBgBlit) {
                    if (CKwin_GetCSISequence(bgSequence, sizeof(bgSequence), currentBgBlit, CK_COLOR_TARGET_BACKGROUND, x, y)) {
                        printf("%s", bgSequence);
                        lastBgBlit = currentBgBlit;
                    }
                }

                // Commit character glyph to hardware stream out
                putchar(currentRawChar);

                // Log values safely within historical memory tracking grids
                historicalCharBuffer[y][x] = currentRawChar;
                historicalTextBlit[y][x]   = currentTextBlit;
                historicalBgBlit[y][x]     = currentBgBlit;
            }
        }
    }

    // Resolve platform hardware console coordinates using the refined properties
    CKwindowNode* activeWin = g_current;
    if (activeWin != NULL) {
        int absoluteCursorX = activeWin->windowPos.startX + activeWin->cursorPos.x;
        int absoluteCursorY = activeWin->windowPos.startY + activeWin->cursorPos.y;

        if (absoluteCursorX >= 0 && absoluteCursorX < CK_COLS &&
            absoluteCursorY >= 0 && absoluteCursorY < CK_ROWS) {
            
            printf("\x1b[%d;%dH", absoluteCursorY + 1, absoluteCursorX + 1);
            
            if (activeWin->cursorBlink) {
                printf("\x1b[?25h"); 
            } else {
                printf("\x1b[?25l"); 
            }
        }
    }


    fflush(stdout);
    CK_ENV.termPending = false;
}

#endif // _WIN32
