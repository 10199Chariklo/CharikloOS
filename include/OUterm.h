#ifndef OU_TERM_H
#define OU_TERM_H

/* Chariklo Subsystem */
    /* Distant module headers */
#include "CKtypes.h"

// Public function fingerprints
    /**
    * @brief Writes text at the current cursor position using active text/bg colors.
    * @param text The null-terminated string to draw to the terminal screen.
    */
void CKwrite(const char* text);

    /**
    * @brief Blits text with specific per-character foreground and background colors.
    * @param text The exact string characters to write.
    * @param textColors A string of blit hex characters ('0'-'f') specifying text color.
    * @param bgColors A string of blit hex characters ('0'-'f') specifying background color. */
void CKblit(const char* text, const char* textColors, const char* bgColors);

/**
 * @brief Public high-level formatted text printer. Serializes text straight
 *        into the current focus window using the underlying CKio stream pipeline.
 * @param format Null-terminated format specifier string matching C printf rules.
 * @return Total characters successfully processed and committed.
 */
int CKprint(const char* format, ...);

    /**
    * @brief Gets the position of the cursor.
    * @arg{ x } For holding x position of the cursor.
    * @arg{ y } For holding y position of the cursor. */
void CKgetCursorPos(int *x, int *y);

    /**
    * @brief Sets the position of the cursor.
    * @arg{ x } The new x position of the cursor.
    * @arg{ y } The new y position of the cursor. */
void CKsetCursorPos(const int x, const int y);

    /**
    * @arg{ state } For holding the blink state of the cursor.
    * @return False on CKcciUpdate failure. */
bool CKgetCursorBlink(bool *state);

    /**
    * @arg{ state } The new blink state of the cursor.
    * @return true on success, false on failure. */
void CKsetCursorBlink(const bool blink);

    /**
    * @brief Queries the grid dimensions of the terminal container.
    * @arg{ x } For holding x width of the terminal.
    * @arg{ y } For holding y height of the terminal. */
void CKgetSize(int *x, int *y);

    /**
    * @brief Clears the target terminal matrix completely using the active background attributes.
    * @return true on success, false on failure.                                                    */
bool CKclear(void);

    /**
    * @return true on success, false on failure.    */
bool CKclearLine(void);

    /**
    * @brief Extracts the currently active text foreground color flags. */
unsigned int CKgetTextColor(void);

    /**
    * @brief Sets the console foreground cell color attributes.
    * @param colorMask The target attribute.
    * @return true on success, false if OS update fails.        */
bool CKsetTextColor(unsigned int colorsMask);

    /**
    * @brief Extracts the currently active screen background color flags. */
unsigned int CKgetBackgroundColor(void);

    /**
    * @brief Sets the console background cell color attributes.
    * @param colorMask The target attribute.
    * @return true on success, false if OS update fails.        */
bool CKsetBackgroundColor(unsigned int colorsMask);

#endif // OU_TERM_H