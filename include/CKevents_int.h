#ifndef CK_EVENTS_INT_H
#define CK_EVENTS_INT_H

/* Standard C11 Libraries */
#include <stdbool.h>

// for something like the ComputerCraft os.pullEvent to work in C our 'Events' need to be a single returnable 'thing'
// This is the file that defines those things, and such, the types of events that can be returned.

// All event types become the same size in memory

/**
 * @brief Enumeration of all event types matching CraftOS paradigms.
 * This acts as our runtime type descriptor.
 */
typedef enum {
    CK_EVENT_NONE = 0,
    CK_EVENT_ALARM,
    CK_EVENT_CHAR,
    CK_EVENT_KEY,
    CK_EVENT_KEY_UP
} CKeventType;

/* Individual event payload definitions */

typedef struct {
    CKeventType type; // Will be set to CK_EVENT_ALARM
    int id;
} CKalarmEvent;

typedef struct {
    CKeventType type; // Will be set to CK_EVENT_CHAR
    char keyPress;
} CKcharEvent;

typedef struct {
    CKeventType type; // Will be set to CK_EVENT_KEY
    int keyPress;
    bool held;
} CKkeyEvent;

typedef struct {
    CKeventType type; // Will be set to CK_EVENT_KEY_UP
    int keyRelease;
} CKkeyUpEvent;

/**
 * @brief Master Tagged Union for all CharikloOS events.
 * This guarantees every event is identical in memory size, safely allowing
 * storage inside our upcoming sequential queue (CKdeque).
 */
typedef union {
    CKeventType type; /* Common initial member to poll the event type safely */
    CKalarmEvent alarm;
    CKcharEvent  character;
    CKkeyEvent   key;
    CKkeyUpEvent keyUp;
} CKevent_t;

/**
 * @brief Compile-time validation utility function.
 * Since default string initializers aren't native to C types, this maps 
 * our internal enum types back to your friendly CraftOS public strings.
 */
static inline const char* CKevent_getName(CKeventType type) {
    switch(type) {
        case CK_EVENT_ALARM:  return "CKalarm";
        case CK_EVENT_CHAR:   return "CKchar";
        case CK_EVENT_KEY:    return "CKkey";
        case CK_EVENT_KEY_UP: return "CKkeyUp";
        default:              return "unknown";
    }
}

#endif /* CK_EVENTS_INT_H */