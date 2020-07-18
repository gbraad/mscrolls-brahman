/*******************************************************************************
 * Stefan Bylund 2019
 *
 * Module for printing the status in the status bar.
 ******************************************************************************/

#ifndef _STATUS_BAR_H
#define _STATUS_BAR_H

#include <font/fzx.h>
#include <stdint.h>

void init_status_bar(struct fzx_font *status_font);

void print_status(uint8_t *room_name, uint8_t *score_turn);

#endif
