/* Standard C11 Libraries */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/* Chariklo Subsystem */
#include "CKio.h" // Local module header
#include "CKterminal_int.h"
#include "CKpalette_int.h"
#include "OUwindow.h"

#define CK_IO_PARSING_LIMIT 1024

// Concrete instances for the static system streams
static struct CKfile lowLevelStdin  = { .streamType = CK_STREAM_STDIN,  .hostFilePointer = NULL, .isOpen = true, .mode = "r" };
static struct CKfile lowLevelStdout = { .streamType = CK_STREAM_STDOUT, .hostFilePointer = NULL, .isOpen = true, .mode = "w" };
static struct CKfile lowLevelStderr = { .streamType = CK_STREAM_STDERR, .hostFilePointer = NULL, .isOpen = true, .mode = "w" };

// Assign public handles
CKfileHandle CKstdin  = (CKfileHandle)&lowLevelStdin;
CKfileHandle CKstdout = (CKfileHandle)&lowLevelStdout;
CKfileHandle CKstderr = (CKfileHandle)&lowLevelStderr;

// Dynamic Stream Redirection Registers (Matches io.input / io.output routing loops)
static CKfileHandle g_CKcurrentInput  = (CKfileHandle)&lowLevelStdin;
static CKfileHandle g_CKcurrentOutput = (CKfileHandle)&lowLevelStdout;

// --- CraftOS Parity+ Implementation Operations ---

const char* CKtype(CKfileHandle handle) {
    // C Idiom: Enforce rigorous pointer and state checking.
    if (handle == NULL) return NULL;
    struct CKfile* file = (struct CKfile*)handle;
    
    // Safety check: returns "closed file" if flag indicates it's unmapped/inactive
    return file->isOpen ? "file" : "closed file";
}

CKfileHandle CKopen(const char* filename, const char* mode) {
    if (filename == NULL || mode == NULL) {
        // TODO: CKsetLastError(CK_ERROR_INVALID_PARAMETER);
        return NULL;
    }

    // Securely allocate a container for our new virtual file stream
    struct CKfile* newFile = (struct CKfile*)malloc(sizeof(struct CKfile));
    if (newFile == NULL) {
        // TODO: CKsetLastError(CK_ERROR_OUT_OF_MEMORY);
        return NULL;
    }

    // TODO Architecture Note: When transitioning away from the hosted MVP, 
    // this block will be swapped entirely for Chariklo's internal virtual FS (CKfs) pipeline.
    FILE* hostFile = NULL;
    if (fopen_s(&hostFile, filename, mode) != 0 || hostFile == NULL) {
        // TODO: CKsetLastError(CK_ERROR_FILE_NOT_FOUND);
        free(newFile);
        return NULL;
    }

    newFile->streamType = CK_STREAM_DISK_FILE;
    newFile->hostFilePointer = hostFile;
    newFile->isOpen = true;
    strcpy_s(newFile->mode, sizeof(newFile->mode), mode);

    return (CKfileHandle)newFile;
}

bool CKclose(CKfileHandle handle) {
    if (handle == NULL) return false;
    struct CKfile* file = (struct CKfile*)handle;

    // Standard streams should never be closed under normal operation
    if (file->streamType == CK_STREAM_STDIN || 
        file->streamType == CK_STREAM_STDOUT || 
        file->streamType == CK_STREAM_STDERR) {
        return false;
    }

    // Strict state check: if already false, fail immediately
    if (!file->isOpen) return false;

    if (file->hostFilePointer != NULL) {
        fclose(file->hostFilePointer);
        file->hostFilePointer = NULL;
    }

    file->isOpen = false;

    // DANGER HOUSEKEEPING: If the file being closed is actively tracked by the dynamic 
    // input/output routing registers, reset them back to the safe system fallback streams 
    // to completely prevent dangling pointer exploitation.
    if (g_CKcurrentInput == handle)  g_CKcurrentInput  = (CKfileHandle)&lowLevelStdin;
    if (g_CKcurrentOutput == handle) g_CKcurrentOutput = (CKfileHandle)&lowLevelStdout;

    free(file); // Free tracking allocation footprint
    return true;
}

CKfileHandle CKinput(CKfileHandle file) {
    if (file != NULL) {
        struct CKfile* checkFile = (struct CKfile*)file;
        // Rigorous check: do not map register if target handle is dead or uninitialized
        if (checkFile->isOpen) {
            g_CKcurrentInput = file;
        } else {
            // TODO: CKsetLastError(CK_ERROR_INVALID_HANDLE);
            return g_CKcurrentInput;
        }
    }
    return g_CKcurrentInput;
}

CKfileHandle CKoutput(CKfileHandle file) {
    if (file != NULL) {
        struct CKfile* checkFile = (struct CKfile*)file;
        // Rigorous check: do not map register if target handle is dead or uninitialized
        if (checkFile->isOpen) {
            g_CKcurrentOutput = file;
        } else {
            // TODO: CKsetLastError(CK_ERROR_INVALID_HANDLE);
            return g_CKcurrentOutput;
        }
    }
    return g_CKcurrentOutput;
}

// --- Stream Primitive Bindings ---
    /**/
int CKwritef(const void* buffer, size_t count) {
    // Context-Aware Routing: Instantly pass tracking off to the current active workspace 
    return CKfwritef(g_current, buffer, count);
}

bool CKflush(CKfileHandle stream) {
    if (stream == NULL) return false;
    struct CKfile* file = (struct CKfile*)stream;
    if (!file->isOpen) return false;

    // Direct synchronization hook: If rendering stdout or stderr, flush out via our hardware HAL
    if (file->streamType == CK_STREAM_STDOUT || file->streamType == CK_STREAM_STDERR) {
        if (CK_ENV.termPending) {
            CKplatformRefresh();
        }
        return true;
    }

    // Disk caching fallback step
    if (file->hostFilePointer != NULL) {
        return (fflush(file->hostFilePointer) == 0);
    }

    return true;
}

// --- High-Level Print Format Drivers ---
// TO DO: Migrate into a dedicated module for our formated print engines exactly as CC does
    /**/
int CKprintf(const char* format, ...) {
    if (format == NULL) return 0;

    char scratchPad[CK_IO_PARSING_LIMIT];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(scratchPad, sizeof(scratchPad), format, args);
    va_end(args);

    if (length > 0) {
        struct CKfile* out = (struct CKfile*)g_CKcurrentOutput;
        
        // State validation check before stream dispatch
        if (!out->isOpen) {
            // TODO: CKsetLastError(CK_ERROR_INVALID_HANDLE);
            return 0;
        }
        // Routes directly to the active system focus node
        if (out->streamType == CK_STREAM_STDOUT || out->streamType == CK_STREAM_STDERR) {
            CKwritef(scratchPad, (size_t)length); 
        }
    }
    return length;
}
    /**/
int CKfprintf(CKfileHandle stream, const char* format, ...) {
    if (stream == NULL || format == NULL) return 0;
    struct CKfile* targetStream = (struct CKfile*)stream;
    if (!targetStream->isOpen) return 0;

    char scratchPad[CK_IO_PARSING_LIMIT];
    va_list args;
    va_start(args, format);
    int length = vsnprintf(scratchPad, sizeof(scratchPad), format, args);
    va_end(args);

    if (length > 0) {
        if (targetStream->streamType == CK_STREAM_STDOUT) {
            CKwritef(scratchPad, (size_t)length);
        } else if (targetStream->streamType == CK_STREAM_STDERR) {
            // Exception stream intercept: Automatically switch text color to Red ('e') for diagnostics
            CKwindowNode* win = g_current;
            unsigned int prevColor = win ? win->activeTextColor : 0x0001;
            
            if (win) win->activeTextColor = 0x4000; // Red mask index
            CKwritef(scratchPad, (size_t)length);
            if (win) win->activeTextColor = prevColor; // Safe tracking restore
        } else if (targetStream->hostFilePointer != NULL) {
            // Traditional storage backup layer: Strip out once fully bound to visual compositor pipelines.
            fwrite(scratchPad, 1, length, targetStream->hostFilePointer);
        }
    }
    return length;
}