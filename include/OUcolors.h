#ifndef CK_COLORS_H
#define CK_COLORS_H

/* Chariklo Subsystem */
    /* Distant module headers */
#include "CKtypes.h"

// Public structure containing color IDs for API/Shell integration
typedef struct {
    unsigned int white;
    unsigned int orange;
    unsigned int magenta;
    unsigned int lightBlue;
    unsigned int yellow;
    unsigned int lime;
    unsigned int pink;
    unsigned int grey;
    unsigned int lightGrey;
    unsigned int cyan;
    unsigned int purple;
    unsigned int blue;
    unsigned int brown;
    unsigned int green;
    unsigned int red;
    unsigned int black;
} CKcolorsPublicInterface;
extern const CKcolorsPublicInterface CKcolors;

// Public functions

/**
 * @brief Checks if a specific color bitmask is present inside a combined color mask.
 */
bool CKtest(unsigned int colorsMask, unsigned int colorTarget);
    
/**
 * @brief Translates an internal Chariklo color string key into its corresponding blit character.
 * @return The character representing that color ('0'-'f'), or ' ' if not found.
 */
char CKtoBlit(const char* keyName);

/**
 * @brief Translates a single blit character color index into its corresponding public string key.
 */
const char* CKfromBlit(char blitChar);

/**
 * @brief Combines two or more color bitmasks using a fast, type-safe inline bitwise OR.
 */
static inline unsigned int CKcombine(unsigned int colorA, unsigned int colorB) {
    return colorA | colorB;
}

/**
 * @brief Subtracts one or more color bitmasks from a base color mask using a fast bitwise AND-NOT.
 */
static inline unsigned int CKsubtract(unsigned int baseColor, unsigned int colorToRemove) {
    return baseColor & ~colorToRemove;
}

#endif // CK_COLORS_H