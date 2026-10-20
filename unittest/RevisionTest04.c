/* ==========================================================================
   CharikloOS Validation Harness: Revision Test 04
   Target: Verification of Geometric Metrics, CK_ENV, and Stream Routing
   ========================================================================== */

#include <windows.h>

#include <conio.h>  // Used for non-blocking keyboard input loop hooks on Windows

/* Chariklo Framework Subsystem Abstractions */
#include "CKterminal_int.h"
#include "OUwindow.h"
#include "CKsetup.h"
#include "CKtypes.h"
#include "CKio.h"
#include "OUterm.h"

void renderPaletteDiagnostics(CKwindowNode* win) {
    if (win == NULL) return;
    
    // Switch dynamic registers to track localized diagnostic text drawing
    CKwindowHandle prevFocus = (CKwindowHandle)CK_ENV.currentWindowNode;
    CK_ENV.currentWindowNode = win;
    
    CKclear();
    CKsetTextColor(0x0010); // Yellow
    CKprintf("= PALETTE DECK =");
    
    CKsetCursorPos(0, 2);
    CKsetTextColor(0x0001); // White
    CKprintf("Base Indices:\n");
    
    // Cycle characters to print raw color matrix codes directly
    CKprintf("0 1 2 3 4 5 6 7\n");
    CKprintf("8 9 a b c d e f\n\n");
    
    CKsetTextColor(0x0020); // Lime
    CKprintf("Alpha Channel:\n");
    CKprintf("'g' Pass-Thru");
    
    // Restore systemic workspace focus
    CK_ENV.currentWindowNode = (CKwindowNode*)prevFocus;
}

void renderWorkspaceStatus(CKwindowNode* win) {
    if (win == NULL) return;
    
    CKwindowHandle prevFocus = (CKwindowHandle)CK_ENV.currentWindowNode;
    CK_ENV.currentWindowNode = win;
    
    CKclear();
    CKsetTextColor(0x0200); // Cyan
    CKprintf("=== CHARIKLO KERNEL WORKSPACE ===\n\n");
    
    CKsetTextColor(0x0001); // White
    CKprintf("System State:  ");
    CKsetTextColor(0x2000); // Green
    CKprintf("RUNNING\n");
    
    CKsetTextColor(0x0001);
    CKprintf("Active Ticks:  %u\n", CK_ENV.activeSystemTick);
    CKprintf("Buffer Layout: Single Absolute Master\n\n");
    
    CKsetTextColor(0x0080); // Grey
    CKprintf("[T] Trigger System Clock Tick\n");
    CKprintf("[E] Emit Diagnostic Error Alert\n");
    CKprintf("[ESC] Halt Test Environment");
    
    CK_ENV.currentWindowNode = (CKwindowNode*)prevFocus;
}

// We use the WINAPI WinMain entry point for Windows compiling
// We only have a windows implemention so we don't have to guard it
// This allows eventual implement boot args when in a hosted enviroment 
// Most hosts provide application args in a similar manner

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    // 1. Initialize Hardware Console Layers via host boot HAL
    if (!CKconsoleSetup()) {
        fprintf(stderr, "[FATAL] Boot HAL rejected hardware console allocation.\n");
        return 1;
    }

    // 2. Establish Workspace Viewports using our 5-parameter signature bounding box
    // Main system state arena
    CKwindowNode* winSystem  = (CKwindowNode*)OUcreate((CKwindowHandle)CK_ENV.nativeWindowNode, 2, 1, 46, 16);
    // Color diagnostic view
    CKwindowNode* winPalette = (CKwindowNode*)OUcreate((CKwindowHandle)CK_ENV.nativeWindowNode, 50, 1, 28, 16);
    // Diagnostic logging deck along the base
    CKwindowNode* winLog     = (CKwindowNode*)OUcreate((CKwindowHandle)CK_ENV.nativeWindowNode, 2, 18, 76, 11);

    if (!winSystem || !winPalette || !winLog) {
        fprintf(stderr, "[FATAL] Viewport allocation failed during boundary calculation.\n");
        CKconsoleTeardown();
        return 1;
    }

    // Configure window state background traits using system primitives
    winSystem->activeBackgroundColor  = 0x8000; // Black
    winPalette->activeBackgroundColor = 0x8000; // Black
    winLog->activeBackgroundColor     = 0x0080; // Dark Grey layout canvas

    // 3. Log System Startup Diagnostics straight to the logging window via CKstderr
    // Temporarily point environment registers to target the logs
    CK_ENV.currentWindowNode = winLog;
    CKclear();
    CKfprintf(CKstderr, "[SYS INIT] Virtual resource matrices allocated securely.\n");
    CKfprintf(CKstderr, "[SYS INIT] Topology Tree: Native -> (System | Palette | Log)\n");

    // 4. Primary Interactive Testing Loop
    bool active = true;
    while (active) {
        // Refresh display matrices
        renderWorkspaceStatus(winSystem);
        renderPaletteDiagnostics(winPalette);
        
        // Force the single absolute canvas to composite updates out to the hardware terminal
        CKflush(CKstdout);
        
        // Await keyboard input from host operating loop hooks
        if (_kbhit()) {
            int inputKey = _getch();
            
            switch (inputKey) {
                case 27: // ESC key hit
                    active = false;
                    break;
                    
                case 't':
                case 'T':
                    // Update environment registers clock tracking flags
                    CK_ENV.activeSystemTick++;
                    CK_ENV.currentWindowNode = winLog;
                    CKprintf("[SYS TICK %03u] Clock cycle advanced successfully.\n", CK_ENV.activeSystemTick);
                    break;
                    
                case 'e':
                case 'E':
                    // Force a runtime diagnostic intercept down the error stream pipe
                    CK_ENV.currentWindowNode = winLog;
                    CKfprintf(CKstderr, "[SYS ERR] Simulated kernel diagnostic alert pushed downstream!\n");
                    break;
                    
                default:
                    break;
            }
        }
    }

    // 5. Deallocate Resources and Clean Memory Lifecycles
    // Tree nodes safely detach themselves from parent rings upon destruction
    CKterminal_FreeWindowNode(winSystem);
    CKterminal_FreeWindowNode(winPalette);
    CKterminal_FreeWindowNode(winLog);

    // Teardown the base environment chassis 
    CKconsoleTeardown();
    return 0;
}
