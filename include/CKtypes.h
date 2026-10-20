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

/* Public Spatial Abstractions */
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