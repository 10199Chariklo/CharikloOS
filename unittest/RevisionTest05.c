/* ==========================================================================
   CharikloOS Validation Harness: Revision Test 05
   Target: Verification of 4-way types module split, Dual-buffer master 
           terminal structure, sparse tree mapping, and vertical topological 
           transparency ('g') crawling passes.
   ========================================================================== */

#include <windows.h>
#include <conio.h>  
#include <stdio.h>

/* Chariklo Framework Subsystem Abstractions */
#include "CKterminal_int.h"
#include "CKtypes_int.h"
#include "OUwindow.h"
#include "CKsetup.h"
#include "CKtypes.h"
#include "OUterm.h"
#include "OUcolors.h"

void renderPaletteDiagnostics(CKwindowNode* win) {
    if (win == NULL) return;
    
    CKwindowNode* prevFocus = g_current;
    CK_ENV.currentWindowNode = win;
    
    CKclear();
    CKsetTextColor(CKcolors.yellow);
    CKprint("= PALETTE DECK =");
    
    // CraftOS style 1-indexed console spacing parameters
    CKsetCursorPos(1, 3);
    CKsetTextColor(CKcolors.white);
    CKprint("Base Indices:\n");
    CKprint("0 1 2 3 4 5 6 7\n");
    CKprint("8 9 a b c d e f\n\n");

    CKsetTextColor(CKcolors.lime);
    CKsetBackgroundColor(CKcolors.lightBlue);
    CKprint("Alpha Channel:\n");
    CKprint("'g' Pass-Thru");
    CKsetBackgroundColor(CKcolors.black);    

    CK_ENV.currentWindowNode = prevFocus;
}

void renderWorkspaceStatus(CKwindowNode* win) {
    if (win == NULL) return;
    
    CKwindowNode* prevFocus = g_current;
    CK_ENV.currentWindowNode = win;
    
    CKclear();
    CKsetTextColor(CKcolors.cyan);
    CKprint("=== CHARIKLO KERNEL WORKSPACE ===\n\n");
    
    CKsetTextColor(CKcolors.white);
    CKprint("System State:  ");
    CKsetTextColor(CKcolors.green);
    CKprint("RUNNING\n");
    
    CKsetTextColor(CKcolors.white);
    CKprint("Active Ticks:  %u\n", CK_ENV.activeSystemTick);
    
    // Explicit Validation Status Strings
    CKprint("Memory Footprint: O(2) Dual-Buffer\n");
    CKprint("Type Boundaries:  4-Way Partition\n\n");
    
    CKsetTextColor(CKcolors.grey);
    CKprint("[T] Trigger System Clock Tick\n");
    CKprint("[E / D ] Emit Diagnostic / Dump Diag\n");
    CKprint("[ESC] Halt Test Environment");
    
    CK_ENV.currentWindowNode = prevFocus;
}

// TUTOR'S TWIST: Overlapping validation layer to physically stress-test 
// the vertical ancestor pixel-crawler ('g') built in Revision 04.
void renderTransparencyOverlay(CKwindowNode* win) {
    if (win == NULL) return;

    CKwindowNode* prevFocus = g_current;
    CK_ENV.currentWindowNode = win;

    CKclear();
    
    // We blit a mixed sequence. 'g' values let the underlying blue / black background 
    // of winSystem and winPalette show through, while text layers paint over top!
    CKblit(" [OVERLAY LAYER] ", "00000000000000000", "ggggggggggggggggg");
    CKsetCursorPos(1, 2);
    CKblit("  Tree Pass-Thru ", "11111111111111111", "ggggggggggggggggg");

    CK_ENV.currentWindowNode = prevFocus;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    // 1. Initialize Hardware Console Layers via host boot HAL
    if (!CKconsoleSetup()) {
        MessageBoxA(NULL, "HAL initialization reject.", "Chariklo Collapse", MB_ICONERROR | MB_OK);
        return 1;
    }

    // 2. Establish Workspace Viewports using our 5-parameter signature bounding box
    // Root container (CK_ENV.nativeWindowNode) acts as parent frame
    CKwindowHandle root = (CKwindowHandle)CK_ENV.nativeWindowNode;

    CKwindowNode* winSystem  = (CKwindowNode*)OUcreate(root, 2,  1, 46, 16);
    CKwindowNode* winPalette = (CKwindowNode*)OUcreate(root, 50, 1, 28, 16);
    
    CKwindowNode* winOverlay = (CKwindowNode*)OUcreate(root, 40, 6, 19, 3);
    
    CKwindowNode* winLog     = (CKwindowNode*)OUcreate(root, 2,  18, 76, 11);

    // Configure structural background properties using updated color masks
    winLog->activeBackgroundColor     = CKcolors.grey;     

    // 3. Log System Startup Diagnostics straight to the logging canvas view
    CKwindowNode* prevFocus = g_current;
    CK_ENV.currentWindowNode = winLog;
    CKclear();
    CKprint("[SYS INIT] Virtual resource matrices allocated securely.\n");
    CKprint("[SYS INIT] Topology Tree: Native -> (System | Palette | Overlay | Log)\n");
    CKprint("[SYS INIT] Verification Target: 4-Way Split & O(2) Memory Pipelines.\n");
    CK_ENV.currentWindowNode = prevFocus;

    // 4. Primary Interactive Testing Loop
    bool active = true;
    while (active) {
        // Run specific buffer stage operations
        renderWorkspaceStatus(winSystem);
        renderPaletteDiagnostics(winPalette);
        renderTransparencyOverlay(winOverlay);
        
        // Execute Stage 2 (Composition) and Stage 3 (ANSI platform write out)
        CKplatformRefresh();
        
        // Await keyboard input from non-blocking host operating loop hooks
        if (_kbhit()) {
            int inputKey = _getch();
            
            switch (inputKey) {
                case 27: // ESC key hit
                    active = false;
                    break;
                    
                case 't':
                case 'T':
                    CK_ENV.activeSystemTick++;
                    CK_ENV.currentWindowNode = winLog;
                    CKprint("[SYS TICK %03u] Clock cycle advanced successfully.\n", CK_ENV.activeSystemTick);
                    break;
                    
                case 'e':
                case 'E':
                    CK_ENV.currentWindowNode = winLog;
                    CKprint("[SYS ALERT] Simulated kernel runtime diagnostic pushed to logger!\n");
                    break;
                    
                case 'd':
                case 'D':
                    // Explicitly freeze the composition tree and flush arrays out to text storage
                    CKterminal_DumpState("chariklo_render_dump.txt");
                    
                    // Switch window focus context to log the update
                    CKwindowNode* logContext = (CKwindowNode*)winLog;
                    CKwindowNode* oldFocus = CK_ENV.currentWindowNode;
                    CK_ENV.currentWindowNode = logContext;
                    
                    CKprint("[DIAGNOSTIC] Complete buffer state serialized to 'chariklo_render_dump.txt'\n");
                    CK_ENV.currentWindowNode = oldFocus;
                    break;


                default:
                    break;
            }
        }
        
        // Small yield sleep statement to ease host scheduling loads
        Sleep(10);
    }

    // 5. Deallocate Resources and Clean Memory Lifecycles
    CKterminal_FreeWindowNode(winSystem);
    CKterminal_FreeWindowNode(winPalette);
    CKterminal_FreeWindowNode(winOverlay);
    CKterminal_FreeWindowNode(winLog);

    // Teardown the base environment chassis 
    CKconsoleTeardown();
    return 0;
}
