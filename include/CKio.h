#ifndef CK_IO_H
#define CK_IO_H

// A one-Jump linker exception would instantly transform this header to OU prefixed - stdbool is also located in CKtypes
// Woes to my slow PC - No linker exceptions allowed until we are approching final build states
/* Standard C11 Libraries */
#include <stdbool.h>

// I suppose the ultimate linker exception is always 'forgetting' a header *wink wink* - ... Don't do that lol

/* Chariklo Subsystem */
    /* Distant module headers */
#include "CKtypes.h"

// Publicly expose the file handle as a completely opaque pointer type
typedef struct CKfile* CKfileHandle;

// Standard stream registers mapping safely across Chariklo
extern CKfileHandle CKstdin;
extern CKfileHandle CKstdout;
extern CKfileHandle CKstderr;

// CraftOS Parity+ Lifecycle & Stream Management
const char* CKtype(CKfileHandle handle);
CKfileHandle CKopen(const char* filename, const char* mode);
bool CKclose(CKfileHandle handle);

CKfileHandle CKinput(CKfileHandle file);
CKfileHandle CKoutput(CKfileHandle file);

// Publically expose unformated write 'primitives'
    /**
     * @brief Public high-level dynamic primitive. Context-aware stream writing 
     *        routing directly into the active g_current environment context.
     */
int CKwritef(const void* buffer, size_t count);
    
    // This fingerprint may eventually be implemted into a CK_G or other module to be built
    /**
     * @brief Targeted primitive layout engine. Commits raw data text streams 
     *        directly onto a specific bounded window context coordinates.
     * @param handle Validated internal CKWindowNode reference.
     * @param buffer Raw source text byte stream pointer.
     * @param count Total number of characters to process.
     * @return Total number of bytes successfully bounded and committed.
     */
int CKfwritef(CKwindowNode* handle, const void* buffer, size_t count);

/**
 * @brief Forces a synchronization pass across virtual buffers. If targeting 
 *        stdout/stderr streams, it drives an immediate hardware canvas refresh.
 * @param stream The active target file stream handle to commit.
 * @return True if successful, false if state constraints fail.
 */
bool CKflush(CKfileHandle stream);

// Formatted Output API (Will eventually migrate to higher abstraction layers like CKterm)
    /**/
int CKprintf(const char* format, ...);
    /**/
int CKfprintf(CKfileHandle stream, const char* format, ...);

#endif // CK_IO_H