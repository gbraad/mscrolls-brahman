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

#if USE_ANIM
extern void show_location_image(uint8_t *image_name) __z88dk_fastcall;
#else
extern void show_location_image(uint8_t image_number) __z88dk_fastcall;
#endif

#if USE_ANIM
static uint8_t *image_names[GAME_LAST_IMAGE + 1] =
{
    "bank",
    "banque",
    "barnes",
    "bath",
    "beahol",
    "beaver",
    "bed",
    "brkrom",
    "broom",
    "bumble",
    "bygate",
    "byriv",
    "catter",
    "cedge",
    "cenhal",
    "conser",
    "cooksb",
    "court",
    "croque",
    "dbed",
    "ddine",
    "dkitch",
    "dland",
    "dloung",
    "dormou",
    "dpond",
    "drive",
    "dunge",
    "end",
    "enthal",
    "epond",
    "ewall",
    "frog",
    "frtpal",
    "giant",
    "glade",
    "grove",
    "grthal",
    "guardr",
    "hatrom",
    "hedge",
    "house",
    "inriv",
    "insky",
    "intea",
    "jury1",
    "jury2",
    "jury3",
    "jury4",
    "lane",
    "lane2",
    "ledge",
    "limb",
    "lounge",
    "maddin",
    "madfoo",
    "madkit",
    "music",
    "nest",
    "npond",
    "nwall",
    "observ",
    "onbrge",
    "outcel",
    "outdun",
    "outgat",
    "outkit",
    "outpal",
    "palkit",
    "pantry",
    "pass3",
    "passag",
    "path",
    "pland",
    "plough",
    "rabbed",
    "rabhal",
    "rabhse",
    "rabkit",
    "rablan",
    "rbrige",
    "river",
    "secret",
    "shed",
    "sink",
    "spond",
    "sstrem",
    "tailor",
    "throne",
    "tinyt",
    "tree",
    "treehs",
    "tunnel",
    "turtle",
    "underb",
    "utree",
    "vege",
    "waiter",
    "walgdn",
    "wall",
    "well",
    "well1",
    "well2",
    "well3",
    "westhl",
    "wood",
    "wood3",
    "wpond",
    "wwall",
    "wwpa3",
    "wwpa6"
};
#endif

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

#if USE_ANIM
    show_location_image(image_names[image]);
#else
    show_location_image(image);
#endif
}

void run_image_slideshow(void)
{
    layer2_set_clip_window(0, 0, 256, MAX_IMAGE_HEIGHT);

#if USE_ANIM
    show_location_image(image_names[image]);
#else
    show_location_image(image);
#endif

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
