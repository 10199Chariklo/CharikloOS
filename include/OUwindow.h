#ifndef OU_WINDOW_H
#define OU_WINDOW_H

/* Chariklo Subsystem */
#include "CKhandles.h"

/**
 * @brief Spawns a new logical viewport window handle on top of the Virtual Framebuffer.
 *        If a parent is provided, the location is evaluated relative to the parent's origin.
 * @param parent Optional handle to a parent workspace window frame. Can be NULL.
 * @param x Local relative column offset within the workspace.
 * @param y Local relative row offset within the workspace.
 * @param width The column width of the viewport boundary.
 * @param height The row height of the viewport boundary.
 * @return A lightweight tracking handle pointer, or NULL if safety bounds check fails.
 */
CKwindowHandle OUcreate(CKwindowHandle parent, int x, int y, int width, int height);

#endif // OU_WINDOW_H