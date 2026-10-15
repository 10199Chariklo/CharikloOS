#ifndef CK_SETUP_INT_H
#define CK_SETUP_INT_H

/* Welcome to CharikloOS! */

/* Chariklo Subsystem - Private APIs */
#include "CKpalette_int.h"
#include "CKterminal_int.h"

// Windows Subsystem
#if defined(_WIN32) || defined(_WIN32_)
#include <windows.h>

// Internal function forward declares / fingerprints
    /**/
bool CKstreamAssignment(void);
    /**/
bool CKhandleAssignment(void);
    /**/
void CKinitWinPalette(void);

#endif // Windows Subsystem

// Linux - Mac - Raylib - ect - Subsystem 
//#if defined()
// ( Future Projects )
//#endif

#endif // CK_SETUP_INT_H