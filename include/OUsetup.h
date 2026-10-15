#ifndef CK_SETUP_H
#define CK_SETUP_H

/* Welcome to CharikloOS! */

/* Chariklo Subsystem - Public APIs */
#include "CKtypes.h"
#include "CKio.h"
#include "CKwindow.h"
#include "OUterm.h"
#include "OUcolors.h"

// Public function fingerprints
    /**/
bool CKconsoleSetup(void);
    /**/
bool CKconsoleTeardown(void);

#endif // CK_SETUP_H