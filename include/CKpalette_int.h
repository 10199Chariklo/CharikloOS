#ifndef CK_PALETTE_INT_H
#define CK_PALETTE_INT_H

// Standard C11 Libraries
#include <string.h>

/* Chariklo Subsystem */
    /* Distant module headers */
#include "CKtypes.h"

// typedef struct containing header data
typedef struct {
    bool isA; //Achromatic
    bool isMono; //Monochromatic
    bool isBlended; //Blended / prioritizes gradients (eg; VIC-2 | Colodore)
} CKpaletteHeader;

// typedef struct containing single color entry
typedef struct {
    char keyName[32];       // e.g., "lightBlue"
    unsigned int  colorID;  // bitmask ID (e.g., 0x0001, 0x0002, 0x0004)
    char blitChar;          // Blit character (e.g., '0' - 'f')
    char hexColor[8];       // True color - Hex string (e.g., "#99B2F2")
} CKcolorEntry;

typedef struct {
    CKpaletteHeader header;
    union {
        struct {
            CKcolorEntry white;   CKcolorEntry orange;  CKcolorEntry magenta; CKcolorEntry lightBlue;
            CKcolorEntry yellow;  CKcolorEntry lime;    CKcolorEntry pink;    CKcolorEntry grey;
            CKcolorEntry lightGrey; CKcolorEntry cyan;  CKcolorEntry purple;  CKcolorEntry blue;
            CKcolorEntry brown;   CKcolorEntry green;   CKcolorEntry red;     CKcolorEntry black;
        };
        CKcolorEntry data[16]; // Overlay array allowing performance-friendly indexing
    };
} CKcolorsInternalInterface;
extern CKcolorsInternalInterface CKcolorsInt;

// Safe storage location for your shell runtime converter
unsigned int CKcolors_ExecuteCombineRuntime(int argCount, const char* args[]);

// Context target definitions for the CSI translator
typedef enum {
    CK_COLOR_TARGET_FOREGROUND = 38,
    CK_COLOR_TARGET_BACKGROUND = 48
} CKcolorTargetContext;

/**
 * @brief Translates an internal blit character ('0'-'f', or 'g') into a 24-bit ANSI color string.
 * @param destinationBuffer Allocated string array where escape sequence will be written.
 * @param bufferSize Size of target buffer array (Recommended minimum: 32 bytes).
 * @param blitChar The color index character ('0'-'f') or 'g' for transparent pass-through.
 * @param context Target location selector: Foreground (38) or Background (48).
 * @param screenX The absolute master canvas column position (0 to CK_COLS - 1).
 * @param screenY The absolute master canvas row position (0 to CK_ROWS - 1).
 * @return True if translation successfully maps, false if an error occurs.
 */
bool CKwin_GetCSISequence(char* destinationBuffer, size_t bufferSize, char blitChar, 
                           CKcolorTargetContext context, int screenX, int screenY);

#endif // CK_TERMINAL_PALETTE_INT_H
