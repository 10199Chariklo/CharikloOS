#ifndef CK_RECT_UTILS_INT_H
#define CK_RECT_UTILS_INT_H

/* Standard C11 Libraries */
#include <stdbool.h>

/* Chariklo subsystem */
#include "CKtypes.h"

/**
 * @brief Hardened structural boundary check that handles integer overflow.
 */
bool CKrectIsValid(const CKrect* rect);

/**
 * @brief Generates a zero-initialized layout block securely.
 */
CKrect CKrectZero(void);

/**
 * @brief Constructs a safe rectangle. Enforces strict normalization
 *        so that start coordinates are always <= end coordinates.
 */
CKrect CKrectNormalize(int x1, int y1, int x2, int y2);

/**
 * @brief Calculates the bounding box that encapsulates two separate rectangles.
 *        Essential for `CKterminal` to determine the minimum "dirty region" 
 *        pushed to `CKplatformRefresh` during multi-window animations.
CKrect CKrect_Combine(const CKrect* r1, const CKrect* r2); */

/**
 * @brief Evaluates strict intersection in grid space.
 */
bool CKrectIntersects(const CKrect* r1, const CKrect* r2);

/**
 * @brief High-performance structural clipping loop.
 * @return True if a valid overlapping matrix intersection zone exists.
 */
bool CKrectIntersectsClip(const CKrect* src, const CKrect* bounds, CKrect* outResult);

/**
 * @brief Tests containment for points, useful for mouse actions.
 */
bool CKrectContainsPoint(const CKrect* rect, CKpoint point);

/**
 * @brief Tests if container completely encapsulates the target rectangle.
 *        Used by `CKwindow_Create` to verify if a child node is safely bounded
 *        within its parent's hardware viewport limits.
 */
bool CKrectContainsRect(const CKrect* container, const CKrect* target);

/**
 * @brief Shifts a rectangle uniformly along a 2D vector axis.
*/
void CKrectTranslate(CKrect* rect, int deltaX, int deltaY);

/**
 * @brief Evaluates if r1 completely encapsulates/envelops r2.
 * @return True if r2 is entirely inside r1, false otherwise or if either is NULL.
 */
bool CKrectEnvelops(const CKrect* r1, const CKrect* r2);

#endif /* CK_RECT_UTILS_INT_H */
