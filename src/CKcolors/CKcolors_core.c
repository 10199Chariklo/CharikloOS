/* Standard C11 Libraries */
#include <string.h>

/* Chariklo Subsystem */
    /* Local module headers */
#include "OUcolors.h"
    /* Distant module headers */
#include "CKpalette_int.h"

// Public interface definitions mapping safely to standard ComputerCraft bit flags
const CKcolorsPublicInterface CKcolors = {
    .white      = 0x0001, .orange     = 0x0002, .magenta    = 0x0004, .lightBlue  = 0x0008,
    .yellow     = 0x0010, .lime       = 0x0020, .pink       = 0x0040, .grey       = 0x0080,
    .lightGrey  = 0x0100, .cyan       = 0x0200, .purple     = 0x0400, .blue       = 0x0800,
    .brown      = 0x1000, .green      = 0x2000, .red        = 0x4000, .black      = 0x8000
};

bool CKtest(unsigned int colorsMask, unsigned int colorTarget) {
    return (colorsMask & colorTarget) == colorTarget;
}

char CKtoBlit(const char* keyName) {
    if (keyName == NULL) return ' ';

    // Search the internal data interface array for a matching name token
    for (int i = 0; i < 16; i++) {
        if (strcmp(CKcolorsInt.data[i].keyName, keyName) == 0) {
            return CKcolorsInt.data[i].blitChar;
        }
    }
    return ' ';
}

const char* CKfromBlit(char blitChar) {
    // Search the internal data interface array for a matching character token
    for (int i = 0; i < 16; i++) {
        if (CKcolorsInt.data[i].blitChar == blitChar) {
            return CKcolorsInt.data[i].keyName;
        }
    }
    // Ultimate fallback if an invalid token is passed
    return "white";
}
