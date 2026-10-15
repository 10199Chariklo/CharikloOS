#if defined(_WIN32) || defined(_WIN32_)
#include <windows.h>

/* Standard C11 Libraries */
#include <stdio.h>

/* Chariklo Subsystem */
    /* Local module headers */
#include "OUsetup.h"
#include "OUsetup_int.h"

// Platform requirements
CKplatformReqs PR;

// Initialization flag
static bool isInitialized = false;

// PLATFORM Stream files - Not internal CK streams
static FILE *ckCONIN$ = NULL;
static FILE *ckCONOUT$ = NULL;
static FILE *ckCONERR$ = NULL;

// Public functions
    /*  1. Call internal safety checks
        2. Perform AllocConsole, window layout configurations, styling parameters
        3. Return status flag */
bool CKconsoleSetup(void) {
    //Early return if we are initialized already
    if (isInitialized) return true;

    // Console assignment
    if (AllocConsole() == false) {
        MessageBoxA(NULL, "Operation error state during (AllocConsole)", "Chariklo Boot Collapse", MB_ICONERROR | MB_OK);
        return false;
    }

    // Stream assignment
    if (!CKstreamAssignment()) {
        FreeConsole();
        MessageBoxA(NULL, "Operation error state during (freopen_s)", "Chariklo Boot Collapse", MB_ICONERROR | MB_OK);
        return false;
    }

    // Handle assignment
    if (!CKhandleAssignment()) {
        FreeConsole();
        MessageBoxA(NULL, "Operation error state during (GetStdHandle)", "Chariklo Boot Collapse", MB_ICONERROR | MB_OK);
        return false;
    }

    // Force Windows Console Host into Virtual Terminal Processing Mode
    DWORD dwMode = 0;
    if (GetConsoleMode(PR.ckSTD_OUTPUT, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(PR.ckSTD_OUTPUT, dwMode);
    }
    if (GetConsoleMode(PR.ckSTD_INPUT, &dwMode)) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
        SetConsoleMode(PR.ckSTD_INPUT, dwMode);
    }

    CKinitWinPalette();

    //Apply our hardcoded CK_DIMS
    // In Win32, we must resize the physical Window AND the internal Screen Buffer.
    // Temporarily shrink window viewport configuration to prevent OS layout rejections
    SMALL_RECT safetyRect = { 0, 0, 1, 1 };
    SMALL_RECT windowSize = { 0, 0, CK_COLS - 1, CK_ROWS - 1 };
    COORD bufferSize = { CK_COLS, CK_ROWS };
    SetConsoleWindowInfo(PR.ckSTD_OUTPUT, TRUE, &safetyRect);
    SetConsoleScreenBufferSize(PR.ckSTD_OUTPUT, bufferSize);
    SetConsoleWindowInfo(PR.ckSTD_OUTPUT, TRUE, &windowSize);

    // HOUSEKEEPING FIX: Map cleanly to standard CC defaults using our updated PR state fields.
    // Index 0: White foreground. Index 15: Black background (shifted 4 bits left for background lane).
    WORD defaultConsoleAttributes = PR.winMask[0] | (PR.winMask[15] << 4);
    SetConsoleTextAttribute(PR.ckSTD_OUTPUT, defaultConsoleAttributes);

    // STRIPPING WINDOW INTERACTION STYLE FLAGS & FORCING FOREGROUND
    HWND consoleWindowHandle = GetConsoleWindow();
    if (consoleWindowHandle != NULL) {
        // Query the existing Window Style Flags
        LONG_PTR windowStyle = GetWindowLongPtr(consoleWindowHandle, GWL_STYLE);

        // Use bitwise AND-NOT (~ Operators) to strip Maximize, Minimize, and Resizing borders
        windowStyle &= ~WS_MAXIMIZEBOX; // Remove maximize button
        //windowStyle &= ~WS_MINIMIZEBOX; // Remove minimize button (Optional: keep if you want to minimize)
        windowStyle &= ~WS_THICKFRAME;  // Remove the draggable/resizable thick edge frame

        // Push the updated style parameters straight back to the OS Kernel Window Manager
        SetWindowLongPtr(consoleWindowHandle, GWL_STYLE, windowStyle);

        // Trigger a window frame update to redraw the borders safely without breaking dimensions
        SetWindowPos(consoleWindowHandle, NULL, 0, 0, 0, 0, 
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

        ShowWindow(consoleWindowHandle, SW_SHOW);
        SetForegroundWindow(consoleWindowHandle);
        SetFocus(consoleWindowHandle);
    }


    // HOUSEKEEPING: Fire our Virtual Framebuffer allocation sequence
    CKterminal_InitMasterBuffer();

    // 8. CRITICAL ALIGNMENT FIX: Invoke 5-parameter signature bounding box.
    // Roots the fallback desktop chassis at origin coordinates (0, 0)
    CKwindowHandle nativeBootHandle = CKcreate(NULL, 0, 0, CK_COLS, CK_ROWS);
    if (nativeBootHandle == NULL) {
        fprintf(stderr, "[CKsetup Collapse] Critical failure allocating Native Background Node framework.\n");
        return false;
    }

    isInitialized = true;
    return true;
}

    /*  1. Reset standard handles
        2. Clean up streams safely
        3. Call FreeConsole */
bool CKconsoleTeardown(void) {
    // Default project variables
    PR.ckSTD_INPUT = INVALID_HANDLE_VALUE;
    PR.ckSTD_OUTPUT = INVALID_HANDLE_VALUE;
    PR.ckSTD_ERROR = INVALID_HANDLE_VALUE;
    ckCONIN$ = NULL;
    ckCONOUT$ = NULL;
    ckCONERR$ = NULL;

    // Teardown
    FreeConsole();

    // Flip isInitialized
    isInitialized = false;
    return true;
}

/* Internal functions */

bool CKstreamAssignment(void) {
    freopen_s(&ckCONIN$,  "CONIN$",  "r", stdin);
    freopen_s(&ckCONOUT$, "CONOUT$", "w", stdout);
    freopen_s(&ckCONERR$, "CONERR$", "w", stderr);
    // Streams are valid - return true
    if (ckCONIN$    ==  NULL    ||
        ckCONOUT$   ==  NULL    ||
        ckCONERR$   ==  NULL    ) return false;
    return true;
}

bool CKhandleAssignment(void) {
    PR.ckSTD_INPUT = GetStdHandle(STD_INPUT_HANDLE);
    PR.ckSTD_OUTPUT = GetStdHandle(STD_OUTPUT_HANDLE);
    PR.ckSTD_ERROR = GetStdHandle(STD_ERROR_HANDLE);
    // Handles are valid - return true
    if (PR.ckSTD_INPUT     ==  INVALID_HANDLE_VALUE    ||
        PR.ckSTD_OUTPUT    ==  INVALID_HANDLE_VALUE    ||
        PR.ckSTD_ERROR     ==  INVALID_HANDLE_VALUE    ) return false;
    return true;
}

#endif // _WIN32