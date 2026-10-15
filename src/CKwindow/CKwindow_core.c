/* Standard C11 Libraries */
#include <stdio.h>
#include <stdlib.h>

/* Chariklo Subsystem */
    /* Local module headers */
#include "CKwindow.h"
    /* Distant module headers */
#include "CKterminal_int.h"

_Static_assert(sizeof(CKwindowNode) > 0, "Error: CKwindowNode size layout configuration missing");

CKwindowHandle CKcreate(CKwindowHandle parent, int x, int y, int width, int height) {
    // 1. Boundary Metric Validation
    if (width <= 0 || width > CK_MAX_CANVAS_COLS || height <= 0 || height > CK_MAX_CANVAS_ROWS) {
        fprintf(stderr, "[CKwindow Error] Sizing metrics unsupported: %dx%d\n", width, height);
        return NULL;
    }

    CKwindowNode* parentNode = (CKwindowNode*)parent;
    int absoluteX = x;
    int absoluteY = y;

    // 2. Vector Translation Pass
    // If a parent workspace window is provided, offset coordinates relative to its top-left vector
    if (parentNode != NULL) {
        absoluteX += parentNode->windowPos.startX;
        absoluteY += parentNode->windowPos.startY;
    }

    // 3. Internal Node Allocation Invoke
    CKwindowNode* newWindow = CKwindow_Create(absoluteX, absoluteY, width, height);
    if (newWindow == NULL) return NULL;

    // 4. Auto-Capture Hook for Root Engine Initialization
    if (CK_ENV.nativeWindowNode == NULL) {
        CK_ENV.nativeWindowNode = newWindow;
        printf("[CKwindow] FIRST window captured! Anchored securely as Root Native Node (%dx%d).\n", width, height);
        return (CKwindowHandle)newWindow;
    }

    // 5. Hierarchy Tree Stacking
    if (parentNode != NULL) {
        newWindow->parent = parentNode;

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
        // If no parent is supplied but root is active, hook as a sibling under root's parent topology layer
        CKwindowNode* currentSibling = CK_ENV.nativeWindowNode;
        while (currentSibling->nextSibling != NULL) {
            currentSibling = currentSibling->nextSibling;
        }
        currentSibling->nextSibling = newWindow;
        newWindow->prevSibling = currentSibling;
        newWindow->parent = CK_ENV.nativeWindowNode->parent;
    }

    return (CKwindowHandle)newWindow;
}