/*******************************************************************************
 * Stefan Bylund 2020
 *
 * Module for loading and playing an NXA animated image file. An NXA image can
 * also be static in which case it is only loaded and displayed.
 *
 * Note: MMU slots 0, 1 and 2 are temporarily used.
 ******************************************************************************/

#ifndef _ANIMATION_H
#define _ANIMATION_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Animation frame interval used in Magnetic Scrolls games.
 */
#define ANIMATION_FRAME_INTERVAL 5

/*
 * Video line where to start drawing the next
 * animation step in Magnetic Scrolls games.
 */
#define ANIMATION_START_LINE 96

/*
 * True if the loaded animation is ongoing, false otherwise.
 */
extern bool animation_on;

/*
 * Load the specified animated image file.
 * The given 256 bytes buffer is used internally.
 */
void load_animation(const char *filename, uint8_t *buf_256);

/*
 * Perform one step in the animation.
 *
 * If animation_on is true, this function should be called with the
 * frequency specified in the animation header in the animated image file.
 */
void animate(void);

#endif
