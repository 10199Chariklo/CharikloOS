/* Standard C11 Libraries */
#include <string.h>
#include <limits.h>

/* Chariklo subsystem */
#include "CKrectUtils_int.h" // Local module header
#include "CKtypes.h"

bool CKrectIsValid(const CKrect* rect) {
    if (rect == NULL) return false;
    
    // Prevent inverted coordinate regions or extreme values that cause memory overflows
    if (rect->startX > rect->endX || rect->startY > rect->endY) return false;
    
    // Catch potential malicious or corrupted sizing bugs before buffer allocation
    if (rect->startX < -4096 || rect->endX > 4096) return false;
    if (rect->startY < -4096 || rect->endY > 4096) return false;
    
    return true;
}

CKrect CKrectZero(void) {
    CKrect r = { .startX = 0, .startY = 0, .endX = 0, .endY = 0 };
    return r;
}

CKrect CKrectNormalize(int x1, int y1, int x2, int y2) {
    CKrect r;
    
    // 1. Establish structural orientation (Lowest coordinate is ALWAYS Home/Top-Left)
    r.startX = (x1 < x2) ? x1 : x2;
    r.endX   = (x1 > x2) ? x1 : x2;
    r.startY = (y1 < y2) ? y1 : y2;
    r.endY   = (y1 > y2) ? y1 : y2;

    // 2. HARDENED SAFETY LAYER: Prevent extreme arbitrary underflows/overflows
    // Even in a virtual cartesian viewport, we bound coordinates within reasonable bounds
    // to prevent memory destruction down the pipeline.
    if (r.startX < -4096) r.startX = -4096;
    if (r.startY < -4096) r.startY = -4096;
    if (r.endX > 4096)    r.endX   = 4096;
    if (r.endY > 4096)    r.endY   = 4096;

    return r;
}

/** Combining RECTS could lead to odd undefinable behevoir if not strictly gaurded - We will come back to this due to the depth of the rendering system I intend to make
CKrect CKrect_Combine(const CKrect* r1, const CKrect* r2) {
    if (r1 == NULL && r2 == NULL) return CKrect_Zero();
    if (r1 == NULL) return *r2;
    if (r2 == NULL) return *r1;

    CKrect combined;
    combined.startX = (r1->startX < r2->startX) ? r1->startX : r2->startX;
    combined.startY = (r1->startY < r2->startY) ? r1->startY : r2->startY;
    combined.endX   = (r1->endX   > r2->endX)   ? r1->endX   : r2->endX;
    combined.endY   = (r1->endY   > r2->endY)   ? r1->endY   : r2->endY;
    return combined;
}   */

/** */
bool CKrectIntersects(const CKrect* r1, const CKrect* r2) {
    if (r1 == NULL || r2 == NULL) return false;
    return (r1->startX < r2->endX && r1->endX > r2->startX &&
            r1->startY < r2->endY && r1->endY > r2->startY);
}
/** */
bool CKrectIntersectsClip(const CKrect* src, const CKrect* bounds, CKrect* outResult) {
    if (outResult == NULL) return false;

    if (!CKrectIntersects(src, bounds)) {
        *outResult = CKrectZero();
        return false;
    }

    outResult->startX = (src->startX > bounds->startX) ? src->startX : bounds->startX;
    outResult->startY = (src->startY > bounds->startY) ? src->startY : bounds->startY;
    outResult->endX   = (src->endX   < bounds->endX)   ? src->endX   : bounds->endX;
    outResult->endY   = (src->endY   < bounds->endY)   ? src->endY   : bounds->endY;

    return true;
}

/** */
bool CKrectContainsPoint(const CKrect* rect, CKpoint point) {
    if (rect == NULL) return false;
    return (point.x >= rect->startX && point.x < rect->endX &&
            point.y >= rect->startY && point.y < rect->endY);
}
/** */
bool CKrectContainsRect(const CKrect* container, const CKrect* target) {
    if (container == NULL || target == NULL) return false;
    return (target->startX >= container->startX && target->endX <= container->endX &&
            target->startY >= container->startY && target->endY <= container->endY);
}

/** */
bool CKrectEnvelops(const CKrect* r1, const CKrect* r2) {
    if (r1 == NULL || r2 == NULL) return false;
    
    // r2 is enveloped if its start bounds are greater/equal to r1,
    // and its end bounds are lesser/equal to r1.
    return (r2->startX >= r1->startX && r2->endX <= r1->endX &&
            r2->startY >= r1->startY && r2->endY <= r1->endY);
}

/** */
void CKrectTranslate(CKrect* rect, int deltaX, int deltaY) {
    if (rect == NULL) return;
    
    // Explicit overflow checking blocks
    if ((deltaX > 0 && rect->endX > INT_MAX - deltaX) || (deltaX < 0 && rect->startX < INT_MIN - deltaX)) return;
    if ((deltaY > 0 && rect->endY > INT_MAX - deltaY) || (deltaY < 0 && rect->startY < INT_MIN - deltaY)) return;

    rect->startX += deltaX;
    rect->endX   += deltaX;
    rect->startY += deltaY;
    rect->endY   += deltaY;
}   