/* Chariklo Subsystem */
#include "CKterminal_int.h"
#include "CKtypes_int.h"
#include "CKgraphUtils.h"
#include "OUwindow.h" // Local module header

/* C11 Compile-Time Assertion: Enforces complete structure verification across translation units */
_Static_assert(sizeof(CKwindowNode) > 0, "Error: CKwindowNode structural size profile configuration missing");

CKwindowHandle OUcreate(CKwindowHandle parent, int x, int y, int width, int height) {
    if (width <= 0 || width > CK_MAX_CANVAS_COLS || height <= 0 || height > CK_MAX_CANVAS_ROWS) {
        fprintf(stderr, "[CKwindow Error] Sizing metrics unsupported: %dx%d\n", width, height);
        return NULL;
    }

    // 1. Pack loose user inputs straight into a temporary stack-based math blueprint record
    CKrect inputBounds;
    inputBounds.startX = x;
    inputBounds.startY = y;
    inputBounds.endX   = x + width;
    inputBounds.endY   = y + height;

    // 2. Invoke factory passing parent handle natively to configure default alpha states
    CKwindowNode* newWindow = CKwindow_Create(parent, inputBounds);
    if (newWindow == NULL) return NULL;

    // 3. Frame Allocation Routing Optimization
    if (CK_ENV.nativeWindowNode == NULL) {
        // Formally establish the desktop chassis baseline
        CK_ENV.nativeWindowNode = newWindow;
    } else {
        // Modernized Graph Topology Stacking Pass
        // If parent is NULL, we append it as a sibling to the native baseline chassis node
        CKgraphNodeHandle targetParent = (parent != NULL) 
            ? (CKgraphNodeHandle)parent 
            : (CKgraphNodeHandle)CK_ENV.nativeWindowNode;

        // One atomic architectural function replaces your manual while loops entirely!
        CKgraph_AppendChild(targetParent, (CKgraphNodeHandle)newWindow);
    }

    newWindow->isDirty = true;
    CK_ENV.termPending = true;

    return (CKwindowHandle)newWindow;
}