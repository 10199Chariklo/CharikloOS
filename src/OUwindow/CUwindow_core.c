/* Chariklo Subsystem */
#include "CKterminal_int.h"
#include "CKtypes_int.h"
#include "OUwindow.h" // Local module header

/* C11 Compile-Time Assertion: Enforces complete structure verification across translation units */
_Static_assert(sizeof(CKwindowNode) > 0, "Error: CKwindowNode structural size profile configuration missing");

CKwindowHandle OUcreate(CKwindowHandle parent, int x, int y, int width, int height) {
    if (width <= 0 || width > CK_MAX_CANVAS_COLS || height <= 0 || height > CK_MAX_CANVAS_ROWS) {
        fprintf(stderr, "[CKwindow Error] Sizing metrics unsupported: %dx%d\n", width, height);
        return NULL;
    }

    CKwindowNode* parentNode = (CKwindowNode*)parent;

    // 1. Pack loose user inputs straight into a temporary stack-based math blueprint record
    // We treat 'width' and 'height' as relative shifts to find the target end coordinates.
    CKrect inputBounds;
    inputBounds.startX = x;
    inputBounds.startY = y;
    inputBounds.endX   = x + width;
    inputBounds.endY   = y + height;

    // Invoke factory passing parent handle natively to configure default alpha states
    CKwindowNode* newWindow = CKwindow_Create(parent, inputBounds);
    if (newWindow == NULL) return NULL;

    if (CK_ENV.nativeWindowNode == NULL) {
        CK_ENV.nativeWindowNode = newWindow;
        return (CKwindowHandle)newWindow;
    }

    // Standard Topology Tree Stacking Pass
    if (parentNode != NULL) {
        if (parentNode->firstChild == NULL) {
            parentNode->firstChild = newWindow;
        } else {
            CKwindowNode* currentSibling = parentNode->firstChild;
            while (currentSibling->nextSibling != NULL) {
                currentSibling = currentSibling->nextSibling;
            }
            currentSibling->nextSibling = newWindow;
            newWindow->prevSibling = currentSibling;
        }
    } else {
        CKwindowNode* currentSibling = CK_ENV.nativeWindowNode;
        while (currentSibling->nextSibling != NULL) {
            currentSibling = currentSibling->nextSibling;
        }
        currentSibling->nextSibling = newWindow;
        newWindow->prevSibling = currentSibling;
    }

    newWindow->isDirty = true;
    CK_ENV.termPending = true;

    return (CKwindowHandle)newWindow;
}
