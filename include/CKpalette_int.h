#ifndef CK_PALETTE_INT_H
#define CK_PALETTE_INT_H

/* Standard C11 Libraries */
#include <stdbool.h>
#include <stddef.h>

/* Chariklo Subsystem */
#include "CKtypes_int.h"

// Immutable 17th Blit Character representing true transparency layer pass-through
#define BLIT_TRANSPARENT_CHAR 'g'

// typedef struct containing header data
typedef struct {
    bool isA;       //Achromatic
    bool isMono;    //Monochromatic
    bool isBlended; //Blended / prioritizes gradients (eg; VIC-2 | Colodore)
} CKpaletteHeader;

// typedef struct containing single color entry
typedef struct {
    char keyName[32];       // e.g., "lightBlue"
    unsigned int  colorID;  // bitmask ID (e.g., 0x0001, 0x0002, 0x0004)
    char blitChar;          // Blit character (e.g., '0' - 'f')
    char hexColor[8];       // True color - Hex string (e.g., "#99B2F2")
} CKpaletteEntry;

typedef struct {
    CKpaletteHeader header;
    union {
        struct {
            CKpaletteEntry white;      CKpaletteEntry  orange;    CKpaletteEntry  magenta;   CKpaletteEntry  lightBlue;
            CKpaletteEntry  yellow;    CKpaletteEntry  lime;      CKpaletteEntry  pink;      CKpaletteEntry  grey;
            CKpaletteEntry  lightGrey; CKpaletteEntry  cyan;      CKpaletteEntry  purple;    CKpaletteEntry  blue;
            CKpaletteEntry  brown;     CKpaletteEntry  green;     CKpaletteEntry  red;       CKpaletteEntry  black;
        };
        CKpaletteEntry data[16]; // Overlay array allowing performance-friendly indexing
    };
} CKpaletteInterface;
extern CKpaletteInterface CKpalette; // Global internal palette interface for runtime access

// Safe storage location for your shell runtime converter
unsigned int CKcolors_ExecuteCombineRuntime(int argCount, const char* args[]);

// Context target definitions for the CSI translator
typedef enum {
    CK_COLOR_TARGET_FOREGROUND = 38,
    CK_COLOR_TARGET_BACKGROUND = 48
} CKcolorTargetContext;


/* WINDOWS FUNCTIONS FINGERPRINTS */
// TO DO : DEFINE GUARD OR MOVE TO AND IMPLEMENT/CREATE CKpalette_win.h

    /** @brief Windows specific palette setup routine.  */
void CKinitWinPalette(void);

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

#endif /* CK_PALETTE_INT_H */
