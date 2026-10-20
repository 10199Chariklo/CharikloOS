#if defined(_WIN32) || defined(_WIN32_)
#include <windows.h>

/* Standard C11 Libraries */
#include <stdbool.h>
#include <stdio.h>

/* Chariklo Subsystem */
#include "CKpalette_int.h" // Local module header
#include "CKterminal_int.h"

void CKinitWinPalette(void) {
    COLORREF rawColors[16] = {
        RGB(0xF0, 0xF0, 0xF0), RGB(0xF2, 0xB2, 0x33), RGB(0xE5, 0x7F, 0xD8), RGB(0x99, 0xB2, 0xF2),
        RGB(0xDE, 0xDE, 0x6C), RGB(0x7F, 0xCC, 0x19), RGB(0xF2, 0xB2, 0xCC), RGB(0x4C, 0x4C, 0x4C),
        RGB(0x99, 0x99, 0x99), RGB(0x4C, 0x99, 0xB2), RGB(0xB2, 0x66, 0xE5), RGB(0x33, 0x66, 0xCC),
        RGB(0x7F, 0x66, 0x4C), RGB(0x57, 0xA6, 0x4E), RGB(0xCC, 0x4C, 0x4C), RGB(0x11, 0x11, 0x11)
    };

    // Explicit native Win32 text color attribute translation table
    // Maps sequential index (0-15) directly to standard Windows Console attribute combinations
    const WORD winConsoleAttributes[16] = {
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, // 0: white
        FOREGROUND_RED | FOREGROUND_GREEN,                                         // 1: orange (Win32 Dark Yellow)
        FOREGROUND_RED | FOREGROUND_BLUE,                                          // 2: magenta
        FOREGROUND_BLUE | FOREGROUND_INTENSITY,                                    // 3: lightBlue
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,                  // 4: yellow
        FOREGROUND_GREEN | FOREGROUND_INTENSITY,                                   // 5: lime
        FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,                   // 6: pink (Win32 Light Magenta)
        FOREGROUND_INTENSITY,                                                      // 7: grey
        FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,                       // 8: lightGrey (Win32 Standard White)
        FOREGROUND_GREEN | FOREGROUND_BLUE,                                        // 9: cyan
        FOREGROUND_RED | FOREGROUND_BLUE,                                          // a: purple
        FOREGROUND_BLUE,                                                           // b: blue
        FOREGROUND_RED,                                                            // c: brown (Win32 Dark Red)
        FOREGROUND_GREEN,                                                          // d: green
        FOREGROUND_RED | FOREGROUND_INTENSITY,                                     // e: red
        0                                                                          // f: black (No flags set)
    };

    for (int i = 0; i < 16; i++) {
        PR.winColor[i] = rawColors[i];
        PR.winMask[i] = winConsoleAttributes[i];
    }
}

bool CKwin_GetCSISequence(char* destinationBuffer, size_t bufferSize, char blitChar, 
                           CKcolorTargetContext context, int screenX, int screenY) {
    if (destinationBuffer == NULL || bufferSize < 32) return false;

    // Direct Token Fallback Protection
    if (blitChar == BLIT_TRANSPARENT_CHAR) {
        blitChar = (context == CK_COLOR_TARGET_FOREGROUND) ? '0' : 'f'; 
    }

    // Index Matrix Matcher
    int colorIndex = -1;
    for (int i = 0; i < 16; i++) {
        if (CKpalette.data[i].blitChar == blitChar) {
            colorIndex = i;
            break;
        }
    }

    if (colorIndex == -1) {
        if (context == CK_COLOR_TARGET_FOREGROUND) {
            snprintf(destinationBuffer, bufferSize, "\x1b[38;2;240;240;240m"); 
        } else {
            snprintf(destinationBuffer, bufferSize, "\x1b[48;2;17;17;17m");    
        }
        return true;
    }

    // Hex Channel Processor
    unsigned int redChannel = 0, greenChannel = 0, blueChannel = 0;
    if (sscanf_s(CKpalette.data[colorIndex].hexColor, "#%02x%02x%02x", 
                 &redChannel, &greenChannel, &blueChannel) != 3) {
        return false;
    }

    snprintf(destinationBuffer, bufferSize, "\x1b[%d;2;%u;%u;%um", 
             (int)context, redChannel, greenChannel, blueChannel);
             
    return true;
}

#endif /* _WIN32 */