#ifndef CK_TYPES_H
#define CK_TYPES_H

/** * @file CKtypes.h
    * @brief Publicly available shared layout types and utility abstractions.
*/

/* Standard C11 Libraries */
#include <stdbool.h>

/* Chariklo subsystem */
#include "CKhandles.h"

/* Chariklo viewport Boundaries */
#ifndef CK_COLS
#define CK_COLS 80
#define CK_ROWS 30
#endif

// Immutable 17th Blit Character representing true transparency layer pass-through
#define BLIT_TRANSPARENT_CHAR 'g'

// Immutable character glyph representing no entry
#define CK_GLYPH_VOID  ((char)0xFF) 

/* --- Dynamic Acrylic Graph engine | CKgraphUtils enums --- */
// Order flags for linear flattening and visitors
typedef enum {
    CK_PRE_ORDER,   // Parent -> Left Sibling -> Right Sibling (Z-index/Rendering)
    CK_POST_ORDER,  // Left Sibling -> Right Sibling -> Parent (Teardown/Deallocation)
    CK_IN_ORDER     // Left Sibling -> Parent -> Right Sibling (Binary/Sorted Layouts)
} CKtraversalOrder;

/* --- Public Spatial Abstractions --- */

typedef struct {
    int x;
    int y;
} CKpoint;

typedef struct {
    int startX;
    int startY;
    int endX;
    int endY;
} CKrect;

// Chariklo Core Numeric Type Definitions
typedef unsigned char      ck_u8;
typedef unsigned short     ck_u16;
typedef unsigned int       ck_u32;

#endif // CK_TYPES_H