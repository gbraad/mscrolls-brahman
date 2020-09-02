/*******************************************************************************
 * Stefan Bylund 2019
 *
 * Test module for displaying all location images for the selected game.
 * Press the O key to show the next image and the P key to show the previous
 * image. Press any other key to exit the slideshow.
 ******************************************************************************/

#include <input.h>
#include <stdbool.h>
#include <stdint.h>

#include "zconfig.h"
#include "image_slideshow.h"
#include "layer2.h"
#include "ide_friendly.h"

extern void show_location_image(uint8_t image_number) __z88dk_fastcall;

static uint8_t image = 0;

static void toggle_image(bool next) __z88dk_fastcall
{
    if (next)
    {
        image++;
        if (image > GAME_LAST_IMAGE)
        {
            image = 0;
        }
    }
    else
    {
        image--;
        if (image > GAME_LAST_IMAGE)
        {
            image = GAME_LAST_IMAGE;
        }
    }

    show_location_image(image);
}

void run_image_slideshow(void)
{
    layer2_set_clip_window(0, 0, 256, MAX_IMAGE_HEIGHT);
    show_location_image(image);

    while (true)
    {
        uint8_t ascii_code = (uint8_t) in_inkey();

        switch (ascii_code)
        {
            case 0:
                continue;
            case 'o':
                toggle_image(false);
                break;
            case 'p':
                toggle_image(true);
                break;
            default:
                return;
        }

        in_wait_nokey();
    }
}
