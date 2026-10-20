#ifndef CK_HANDLES_H
#define CK_HANDLES_H

/** * @file CKhandles.h
    * @brief Public Handle Layer for the Chariklo OS Simulation Framework.
    * 
    * This module defines the public interface handles used at the scripting and
    * application layers. Handles are split into two conceptual groups to give
    * developers maximum flexibility between explicit context-passing and implicit
    * global state switching.
*/

/*  *   Object handles                                                        
    * DESIGN & USAGE NOTE:
    * 'Object' handles model stateful instances within Chariklo (such as context-driven 
    * terminal states, active layout nodes, or open system file streams).
    * 
    * Developers can manipulate these objects using either explicit or implicit patterns, 
    * depending on personal preference or structural requirements:
    * 
    * 1. Explicit Handle Usage (Direct Method Calling):
    *    Pass or use the handle directly on an instance-focused routine.
    *    Example: CKtermHandle.write("Hello Chariklo"); 
    * 
    * 2. Implicit Handle Usage (Stateful / Global Switching):
    *    Bind the handle as the current context, then call general routines.
    *    Example: 
    *       CKterm.setCurrent(CKtermHandle); 
    *       CKterm.write("Hello Chariklo");
*/

// Forward structural declarations for system objects
typedef struct CKwindowNode   CKwindowNode;
typedef struct CKfile         CKfile;

// Public Object Handle Aliases
typedef struct CKwindowNode*  CKwindowHandle;
typedef struct CKfile*        CKfileHandle;

/*  *   Type handles                                                            
    * DESIGN & USAGE NOTE:
    * 'Type' handles are used for passing raw internal data structures across the
    * scripting/application layer. 
    *
    * If a function requires access to a core structure (like 
    * the Platform Requirements registry) but does not have it within scope, 
    * the developer passes the 'Type' handle to the function. The core engine can 
    * then unpack the underlying data directly.
    * 
    * This is especially useful for:
    * 1. Functions that are not part of the core Chariklo library and lack global scope.
    * 2. Developers implementing custom engine behaviors who need raw structural hooks 
    *    without wanting to include private headers ("CKtypes_int.h") or compile 
    *    against the entire internal codebase.
    *
    * Example:
    *    CKmyModule.setPlatformReqs(CKplatformReqsHandle);
*/

// Forward structural declarations for type safety
typedef struct CKplatformReqs CKplatformReqs;
typedef struct CKenv          CKenv;

// Public Type Handle Aliases
typedef struct CKplatformReqs* CKplatformReqsHandle;
typedef struct CKenv*          CKenvHandle;

/*  * If a module developer included CKhandles.h manually, but CKtypes_int.h 
    * has not been evaluated yet, nudge them toward standard practice.
*/
#ifdef CK_MODULE_DEV
    #ifndef CK_TYPES_INT_H
        #warning "Chariklo Dev Tip: CKtypes_int.h is recommended to be included before CKhandles.h. If you are implementing a new core module, place it at the top of your module's _core.c file before manipulating raw data structures to unlock hidden struct access. You may ignore this warning if you are not manipulating internal raw data structures."
    #endif
#endif

#endif /* CK_HANDLES_H */