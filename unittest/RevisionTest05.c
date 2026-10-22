/* ==========================================================================
   CharikloOS Validation Harness: Revision Test 05 (Memory Jogger Upgrade)
   Target: Verification of 4-way types module split, Dual-buffer master 
           terminal structure, and automated Win32 memory profiling.
   ========================================================================== */

#include <windows.h>
#include <psapi.h>  // Added for GetProcessMemoryInfo
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

// Helper function to extract memory metrics using the Win32 API
void queryMemoryMetrics(SIZE_T* workingSet, SIZE_T* pagefileUsage) {
    HANDLE hProcess = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
        *workingSet = pmc.WorkingSetSize;
        *pagefileUsage = pmc.PagefileUsage;
    } else {
        *workingSet = 0;
        *pagefileUsage = 0;
    }
}

void renderPaletteDiagnostics(CKwindowNode* win) {
    if (win == NULL) return;
    
    CKwindowNode* prevFocus = g_current;
    CK_ENV.currentWindowNode = win;
    
    CKclear();
    CKsetTextColor(CKcolors.yellow);
    CKprint("= PALETTE DECK =");
    
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
    
    CKprint("Memory Footprint: O(2) Dual-Buffer\n");
    CKprint("Type Boundaries:  4-Way Partition\n\n");
    
    CKsetTextColor(CKcolors.grey);
    CKprint("[T] Clock Tick   | [J] Memory Jogger\n");
    CKprint("[E] Emit Alert   | [D] Serialized Dump\n");
    CKprint("[ESC] Halt Test Environment");
    
    CK_ENV.currentWindowNode = prevFocus;
}

void renderTransparencyOverlay(CKwindowNode* win) {
    if (win == NULL) return;

    CKwindowNode* prevFocus = g_current;
    CK_ENV.currentWindowNode = win;

    CKclear();
    CKblit(" [OVERLAY LAYER] ", "00000000000000000", "ggggggggggggggggg");
    CKsetCursorPos(1, 2);
    CKblit("  Tree Pass-Thru ", "11111111111111111", "ggggggggggggggggg");

    CK_ENV.currentWindowNode = prevFocus;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) {
    // 1. Initialize Hardware Console Layers
    if (!CKconsoleSetup()) {
        MessageBoxA(NULL, "HAL initialization reject.", "Chariklo Collapse", MB_ICONERROR | MB_OK);
        return 1;
    }

    // 2. Establish Workspace Viewports
    CKwindowHandle root = (CKwindowHandle)CK_ENV.nativeWindowNode;

    CKwindowNode* winSystem  = (CKwindowNode*)OUcreate(root, 2,  1, 46, 16);
    CKwindowNode* winPalette = (CKwindowNode*)OUcreate(root, 50, 1, 28, 16);
    CKwindowNode* winOverlay = (CKwindowNode*)OUcreate(root, 40, 6, 19, 3);
    CKwindowNode* winLog     = (CKwindowNode*)OUcreate(root, 2,  18, 76, 11);

    winLog->activeBackgroundColor = CKcolors.grey;     

    // 3. Log System Startup Diagnostics
    CKwindowNode* prevFocus = g_current;
    CK_ENV.currentWindowNode = winLog;
    CKclear();
    CKprint("[SYS INIT] Virtual resource matrices allocated securely.\n");
    CKprint("[SYS INIT] Topology Tree: Native -> (System | Palette | Overlay | Log)\n");
    CKprint("[SYS INIT] Verification Target: 4-Way Split & O(2) Memory Pipelines.\n");
    CK_ENV.currentWindowNode = prevFocus;

    // --- JOGGING STATE ENGINE REGISTERS ---
    bool jogging     = false;
    int iterations   = 0;
    int limit        = 0;

    // 4. Primary Interactive Testing Loop
    bool active = true;
    while (active) {
        // Run specific buffer stage operations
        renderWorkspaceStatus(winSystem);
        renderPaletteDiagnostics(winPalette);
        renderTransparencyOverlay(winOverlay);
        
        // Execute Stage 2 (Composition) and Stage 3 (ANSI platform write out)
        CKplatformRefresh();
        
        int inputKey = 0;
        bool processKey = false;

        // INTERCEPT LAYER: If running a jog, bypass physical keyboard polling loops
        if (jogging) {
            if (iterations > 1) {
                inputKey = 'X'; // Silent buffer tick processing pass
            } else if (iterations == 1) {
                inputKey = 'J'; // Automation complete; route back to tear down jogging status
            }
            processKey = true;
        } else {
            // Standard interactive mode polling
            if (_kbhit()) {
                inputKey = _getch();
                processKey = true;
            }
        }
        
        if (processKey) {
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

                case 'x':
                case 'X':
                    // Silent Tick Case: Mutates internal clock register without rendering console prints
                    CK_ENV.activeSystemTick++;
                    if (jogging) {
                        iterations--;
                    }
                    break;
                    
                case 'j':
                case 'J':
                    if (!jogging) {
                        // STAGE 1: Initialize the Jogging Automaton
                        CK_ENV.currentWindowNode = winLog;
                        CKfill(); // Centralized primitive utility execution pass
                        
                        SIZE_T initWorkingSet, initPagefile;
                        queryMemoryMetrics(&initWorkingSet, &initPagefile);

                        CKprint("=== MEMORY JOGGER ACTION INITIALIZED ===\n");
                        CKprint("Base Working Set: %u KB\n", (unsigned int)(initWorkingSet / 1024));
                        CKprint("Base Page File:   %u KB\n", (unsigned int)(initPagefile / 1024));
                        CKprint("----------------------------------------\n");

                        // Configure verification parameters
                        jogging = true;
                        limit = 100;       // Stress limit threshold configurations
                        iterations = 100;  // Target tick duration lifecycle
                    } else {
                        // STAGE 3: Finalize and log results when iterations hit target limits
                        CK_ENV.currentWindowNode = winLog;
                        
                        SIZE_T finalWorkingSet, finalPagefile;
                        queryMemoryMetrics(&finalWorkingSet, &finalPagefile);

                        CKprint("=== MEMORY JOGGER FINAL REPORT ===\n");
                        CKprint("Final Working Set: %u KB\n", (unsigned int)(finalWorkingSet / 1024));
                        CKprint("Final Page File:   %u KB\n", (unsigned int)(finalPagefile / 1024));
                        CKprint("Configured Limit:  %d iterations\n", limit);
                        CKprint("========================================\n");

                        // Deallocate engine states back to idle configurations
                        jogging = false;
                        iterations = 0;
                        limit = 0;
                    }
                    break;
                    
                case 'e':
                case 'E':
                    CK_ENV.currentWindowNode = winLog;
                    CKprint("[SYS ALERT] Simulated kernel runtime diagnostic pushed to logger!\n");
                    break;
                    
                case 'd':
                case 'D':
                    CKterminal_DumpState("chariklo_render_dump.txt");
                    
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

    CKconsoleTeardown();
    return 0;
}
