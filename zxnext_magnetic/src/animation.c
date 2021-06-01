/*******************************************************************************
 * Stefan Bylund 2020
 *
 * Module for loading and playing an NXA animated image file. An NXA image can
 * also be static in which case it is only loaded and displayed.
 *
 * Note: MMU slots 0, 1 and 2 are temporarily used.
 ******************************************************************************/

#include <arch/zxn.h>
#include <arch/zxn/esxdos.h>
#include <intrinsic.h>

#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#include "zconfig.h"
#include "animation.h"
#include "nxa.h"
#include "layer2.h"
#include "layer2_blit.h"

/*
 * The following max values are based on Wonderland and are rounded upwards to
 * be on the safe side. The biggest animation in Wonderland uses 26 MMU pages
 * of frame data.
 */

#define MAX_FRAMES 100
#define MAX_POSITIONS 1000
#define MAX_ANIM_STEPS_SIZE 2000
#define MAX_FRAME_RECTS 10

#define ANIM_VARS_PAGE 8
#define IMAGE_COPY_BASE_PAGE 55
#define FRAME_DATA_BASE_PAGE 60

#define MMU0_ADDRESS ((uint8_t *) 0)
#define MMU2_ADDRESS ((uint8_t *) 0x4000)

#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

#define GET_SCREEN_BASE_PAGE(screen) (ZXN_READ_REG(screen) << 1)

typedef struct frame_rect
{
    uint8_t x;
    uint8_t y;
    uint16_t width;
    uint8_t height;
} frame_rect_t;

bool animation_on = false;

static nxa_image_header_t image_header;

static nxa_anim_header_t anim_header;

/* The animation variables frames, frame_positions and anim_steps are stored in a
 * separate MMU page (ANIM_VARS_PAGE) and paged in to MMU slot 2 (0x4000 - 0x6000)
 * when accessed. The most memory-consuming animation in Wonderland requires less
 * than 6 KB for its animation variables so a single 8 KB MMU page is more than
 * adequate.
 */

static nxa_frame_t *frames = (nxa_frame_t *) 0x4000;

static nxa_frame_position_t *frame_positions = NULL;

static uint8_t *anim_steps = NULL;
static uint16_t anim_step_pos = 0;

static frame_rect_t frame_rects_a[MAX_FRAME_RECTS];
static uint8_t num_frame_rects_a = 0;

static frame_rect_t frame_rects_b[MAX_FRAME_RECTS];
static uint8_t num_frame_rects_b = 0;

static frame_rect_t *frame_rects = frame_rects_b;
static uint8_t *num_frame_rects = &num_frame_rects_b;

static layer2_blit_context_t blit_context;

extern void set_image_window_height(uint8_t image_height);

/*
 * Copy the image from the layer 2 shadow screen to the image copy memory area
 * in max 8 KB chunks via MMU slot 0. Note that if the image width is less than
 * the layer 2 screen width (256 pixels), it is centered horizontally and the
 * full screen width is copied. MMU slot 2 will be used for the actual blitting.
 */
static void blit_image_copy(void)
{
    uint8_t src_screen_base_page = GET_SCREEN_BASE_PAGE(SHADOW_SCREEN);
    uint8_t page = IMAGE_COPY_BASE_PAGE;
    uint8_t y = image_header.y;
    uint8_t height = image_header.height;

    while (height)
    {
        uint8_t section_height;

        ZXN_WRITE_MMU0(page++);

        section_height = 32;
        if (section_height > height)
        {
            section_height = height;
        }

        layer2_blit_screen_section(MMU0_ADDRESS, src_screen_base_page, y, section_height);

        y += section_height;
        height -= section_height;
    }
}

/*
 * Blit a frame from the frame data memory area to the layer 2 shadow screen in
 * max 8 KB chunks via MMU slots 0 and 1. The frame parameter specifies the
 * frame, the frame_position parameter its position and the frame_rect parameter
 * the clipped part of the frame that will be blitted. MMU slot 2 will be used
 * for the actual blitting.
 */
static void blit_frame(nxa_frame_t *frame, nxa_frame_position_t *frame_position, frame_rect_t *frame_rect)
{
    /*
     * NOTE: The frame and frame_position paramaters are located in MMU slot 2.
     * This means that we must access them before the while loop below which
     * calls layer2_blit() or layer2_blit_trans() which also use MMU slot 2.
     */

    uint8_t page = FRAME_DATA_BASE_PAGE + frame->page;
    uint16_t addr = frame->offset;
    uint8_t y = image_header.y + frame_rect->y;
    uint8_t height = frame_rect->height;
    uint8_t frame_transparent = frame->transparent;

    blit_context.src_line_width = frame->width;
    blit_context.x = image_header.x + frame_rect->x;
    blit_context.width = frame_rect->width;
    blit_context.transparency_color = anim_header.transparency_color;

    // Adjust frame page and address if frame position origin is outside of image.
    if (frame_position->x < 0)
    {
        addr -= frame_position->x;
    }
    if (frame_position->y < 0)
    {
        addr -= frame_position->y * blit_context.src_line_width;
    }
    if (addr > 0x2000)
    {
        page += addr / 0x2000;
        addr = addr % 0x2000;
    }

    while (height)
    {
        uint16_t section_size;
        uint16_t section_height;
        uint16_t rest;

        ZXN_WRITE_MMU0(page++); // source page
        ZXN_WRITE_MMU1(page);   // potential spill-over of last section line on next page

        section_size = 0x2000 - addr;
        section_height = section_size / blit_context.src_line_width;
        rest = section_size % blit_context.src_line_width;
        if (rest != 0)
        {
            // Handle spill-over of last section line on next page.
            section_height++;
        }
        if (section_height > height)
        {
            section_height = height;
        }

        // These fields are modified by the called blit function.
        blit_context.src = (uint8_t *) addr;
        blit_context.y = y;
        blit_context.height = section_height;

        if (frame_transparent)
        {
            layer2_blit_trans(&blit_context);
        }
        else
        {
            layer2_blit(&blit_context);
        }

        addr = (rest == 0) ? 0 : (blit_context.src_line_width - rest);
        y += section_height;
        height -= section_height;
    }
}

/*
 * Blit a rectangular area from the image in the image copy memory area to the
 * layer 2 shadow screen in max 8 KB chunks via MMU slot 0. Note that the line
 * width of the stored image is the full layer 2 screen width (256 pixels). MMU
 * slot 2 will be used for the actual blitting.
 */
static void blit_background(frame_rect_t *frame_rect)
{
    uint8_t y = frame_rect->y;
    uint8_t page = IMAGE_COPY_BASE_PAGE + (y >> 5); // (y / 32)
    uint8_t height = frame_rect->height;

    blit_context.src_line_width = 256;
    blit_context.x = image_header.x + frame_rect->x;
    blit_context.src = (uint8_t *) (blit_context.x + (((uint16_t) (y % 32)) << 8));
    blit_context.width = frame_rect->width;

    while (height)
    {
        uint8_t section_height;

        ZXN_WRITE_MMU0(page++);

        section_height = 32 - (y % 32);
        if (section_height > height)
        {
            section_height = height;
        }

        // These fields are modified by the called blit function.
        blit_context.y = image_header.y + y;
        blit_context.height = section_height;

        layer2_blit(&blit_context);

        // Start at first line on next pages, only adjust for the X-offset.
        blit_context.src = (uint8_t *) blit_context.x;
        y += section_height;
        height -= section_height;
    }
}

/*
 * Load main image from file to shadow screen in max 8 KB chunks using MMU slot 2.
 */
static void load_main_image(uint8_t filehandle)
{
    uint8_t page = GET_SCREEN_BASE_PAGE(SHADOW_SCREEN);
    uint16_t size = image_header.size;
    errno = 0;

    while (size > 0)
    {
        uint16_t buffer_size;

        ZXN_WRITE_MMU2(page++);

        buffer_size = 0x2000;
        if (buffer_size > size)
        {
            buffer_size = size;
        }

        esx_f_read(filehandle, MMU2_ADDRESS, buffer_size);
        if (errno)
        {
            return;
        }

        size -= buffer_size;
    }
}

/*
 * Load frame data from file to frame data memory area in max 8 KB chunks using MMU slot 2.
 */
static void load_frame_data(uint8_t filehandle)
{
    uint8_t num_pages;
    uint8_t page;
    uint16_t rest;

    errno = 0;
    num_pages = zxn_page_from_addr(anim_header.frame_data_size); // num_pages = frame_data_size / 0x2000

    for (page = FRAME_DATA_BASE_PAGE; num_pages--; ++page)
    {
        ZXN_WRITE_MMU2(page);
        esx_f_read(filehandle, MMU2_ADDRESS, 0x2000);
        if (errno)
        {
            return;
        }
    }

    // Load the last chunk (or the first and only chunk) if its size is less than 8 KB.

    rest = (uint16_t) (anim_header.frame_data_size % 0x2000);

    if (rest > 0)
    {
        ZXN_WRITE_MMU2(page);
        esx_f_read(filehandle, MMU2_ADDRESS, rest);
    }
}

/*
 * Load the specified animated image file.
 * The given 256 bytes buffer is used internally.
 *
 * Note: MMU slots 0, 1 and 2 are temporarily used and restored
 * to their default values (i.e. the ROM and ULA screen).
 */
void load_animation(const char *filename, uint8_t *buf_256)
{
    uint8_t filehandle;
    layer2_palette_t palette;

    // Note: Caller must ensure that the Spectrum ROM is in place.

    // Reset animation state.
    intrinsic_di();
    animation_on = false;
    intrinsic_ei();
    anim_step_pos = 0;
    num_frame_rects_a = 0;
    num_frame_rects_b = 0;
    frame_rects = frame_rects_b;
    num_frame_rects = &num_frame_rects_b;

    // Open animated image file.
    errno = 0;
    filehandle = esx_f_open(filename, ESX_MODE_R | ESX_MODE_OPEN_EXIST);
    if (errno)
    {
        return;
    }

    // Load image header.
    esx_f_read(filehandle, &image_header, sizeof(nxa_image_header_t));
    if (errno)
    {
        goto end;
    }

    // Load palette colors into the palette not currently used (primary or secondary).
    palette = layer2_get_unused_access_palette();
    esx_f_read(filehandle, buf_256, 256);
    if (errno)
    {
        goto end;
    }
    layer2_set_palette(palette, (uint16_t *) buf_256, 128, 0);
    esx_f_read(filehandle, buf_256, 256);
    if (errno)
    {
        goto end;
    }
    layer2_set_palette(palette, (uint16_t *) buf_256, 128, 128);

    // Load main image to shadow screen.
    load_main_image(filehandle);
    if (errno)
    {
        goto end;
    }

    // Load animation part if available.

    if (image_header.animated)
    {
        uint16_t frames_size;
        uint16_t frame_positions_size;

        // Load animation header.
        esx_f_read(filehandle, &anim_header, sizeof(nxa_anim_header_t));
        if (errno)
        {
            goto end;
        }

        if ((anim_header.num_frames > MAX_FRAMES) ||
            (anim_header.num_positions > MAX_POSITIONS) ||
            (anim_header.steps_size > MAX_ANIM_STEPS_SIZE))
        {
            goto end;
        }

        // The animation variables frames, frame_positions and anim_steps are stored in
        // a separate MMU page ANIM_VARS_PAGE and paged in to MMU slot 2 when accessed.
        ZXN_WRITE_MMU2(ANIM_VARS_PAGE);

        // Load animation frames.
        frames_size = esx_f_read(filehandle, frames, sizeof(nxa_frame_t) * anim_header.num_frames);
        if (errno)
        {
            goto end;
        }

        // Load animation frame positions.
        frame_positions = (nxa_frame_position_t *) (0x4000 + frames_size);
        frame_positions_size = esx_f_read(filehandle, frame_positions, sizeof(nxa_frame_position_t) * anim_header.num_positions);
        if (errno)
        {
            goto end;
        }

        // Load animation steps.
        anim_steps = (uint8_t *) (0x4000 + frames_size + frame_positions_size);
        esx_f_read(filehandle, anim_steps, anim_header.steps_size);
        if (errno)
        {
            goto end;
        }

        // Load animation frame data.
        load_frame_data(filehandle);
        if (errno)
        {
            goto end;
        }

        // Copy main image for later background restoring during animation.
        blit_image_copy();

        // Disable layer 2 screen in case the following doesn't complete before the next display frame is drawn.
        wait_video_line(MAX_IMAGE_HEIGHT);
        layer2_config(false);

        // Change the height of the image window.
        set_image_window_height(image_header.height + 2);

        // Flip the primary/secondary palettes.
        layer2_flip_display_palettes();

        // Copy shadow screen to main screen so both are identical before starting the double-buffered animation.
        layer2_blit_screen(MAIN_SCREEN, SHADOW_SCREEN);

        // Draw first animation step since the main image may be incomplete for some animations.
        animate();

        // Enable layer 2 screen again.
        layer2_config(true);

        // Turn on animation.
        animation_on = true;
    }
    else
    {
        // Flip the main/shadow screens and the primary/secondary palettes.
        wait_video_line(MAX_IMAGE_HEIGHT);
        set_image_window_height(image_header.height + 2);
        layer2_flip_main_shadow_screen();
        layer2_flip_display_palettes();
    }

end:
    // Restore default pages in MMU slots 0, 1 and 2.
    ZXN_WRITE_MMU0(255);
    ZXN_WRITE_MMU1(255);
    ZXN_WRITE_MMU2(10);
    esx_f_close(filehandle);
}

/*
 * Get the next animation step.
 * Returns true if there are remaining animation steps.
 */
static bool get_next_positions(uint16_t **positions, uint8_t *num_positions)
{
    *num_positions = 0;
    *positions = NULL;

    /*
     * Note: This function and the way its return value is used, will always
     * skip the last animation step. This is strange but the animations in
     * Wonderland seem to be designed in this way.
     */

    if (anim_step_pos < anim_header.steps_size)
    {
        *num_positions = anim_steps[anim_step_pos++];
        if (*num_positions > 0)
        {
            *positions = (uint16_t *) &anim_steps[anim_step_pos];
            anim_step_pos += *num_positions << 1;
        }
    }

    if ((anim_step_pos >= anim_header.steps_size) && anim_header.repeating)
    {
        anim_step_pos = 0;
        return get_next_positions(positions, num_positions);
    }

    return (anim_step_pos < anim_header.steps_size);
}

/*
 * Calculate and fill in the given clip rectangle from the given frame and its position.
 * Returns true if the resulting clip rectangle is not empty.
 */
static bool clip_frame_rect(nxa_frame_t *frame, nxa_frame_position_t *frame_position, frame_rect_t *frame_rect)
{
    // Note: Frame coordinates are relative image origin.

    int16_t x_min = (int16_t) MAX(0, frame_position->x);
    int16_t y_min = (int16_t) MAX(0, frame_position->y);
    int16_t x_max = (int16_t) MIN(image_header.width, frame_position->x + frame->width) - 1;
    int16_t y_max = (int16_t) MIN(image_header.height, frame_position->y + frame->height) - 1;

    // Does anything at all remain of the frame?
    if ((y_max < y_min) || (x_max < x_min))
    {
        return false;
    }

    frame_rect->x = (uint8_t) x_min;
    frame_rect->y = (uint8_t) y_min;
    frame_rect->width = (uint16_t) (x_max - x_min + 1);
    frame_rect->height = (uint8_t) (y_max - y_min + 1);

    return true;
}

/*
 * Flip the A and B frame rectangles.
 */
static void flip_frame_rects(void)
{
    if (frame_rects == frame_rects_a)
    {
        frame_rects = frame_rects_b;
        num_frame_rects = &num_frame_rects_b;
    }
    else
    {
        frame_rects = frame_rects_a;
        num_frame_rects = &num_frame_rects_a;
    }
}

/*
 * Perform one step in the animation.
 *
 * If animation_on is true, this function should be called with the
 * frequency specified in the animation header in the animated image file.
 *
 * Note: MMU slots 0, 1 and 2 are temporarily used and must be restored by
 * the caller.
 */
void animate(void)
{
    uint16_t *positions;
    uint8_t num_positions;
    bool has_more;

    /*
     * The animation variables anim_steps (pointed to by the positions local
     * variable), frame_positions and frames are temporarily paged in to MMU
     * slot 2 in this function since they are too large to always keep in main
     * memory. MMU slot 2 is also used temporarily by the blit_frame() function
     * called by this function. This means that special care must be taken when
     * accessing the frame and frame_position parameters passed to blit_frame().
     */

    ZXN_WRITE_MMU2(ANIM_VARS_PAGE);

    // Get the next animation step.
    has_more = get_next_positions(&positions, &num_positions);

    if (!has_more)
    {
        animation_on = false;
        return;
    }

    // Use the shadow screen for the blit_frame() calls below.
    blit_context.screen_base_page = GET_SCREEN_BASE_PAGE(SHADOW_SCREEN);

    *num_frame_rects = 0;

    // Blit the animation frames for this animation step on the shadow screen.
    for (uint16_t i = 0; i < num_positions; i++)
    {
        uint16_t position = positions[i];
        nxa_frame_position_t *frame_position = &frame_positions[position];
        nxa_frame_t *frame = &frames[frame_position->frame_number];
        frame_rect_t *frame_rect = &frame_rects[*num_frame_rects];

        if (clip_frame_rect(frame, frame_position, frame_rect))
        {
            blit_frame(frame, frame_position, frame_rect);
            (*num_frame_rects)++;
            ZXN_WRITE_MMU2(ANIM_VARS_PAGE);
        }
    }

    // If the video line is < ANIMATION_START_LINE, the drawing of this
    // animation step took too long time and we have entered the displaying of
    // the next screen frame. To avoid tearing in this case, we wait until line
    // MAX_IMAGE_HEIGHT before displaying the drawn animation step.
    if ((ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_H) == 0) &&
        (ZXN_READ_REG(REG_ACTIVE_VIDEO_LINE_L) < ANIMATION_START_LINE))
    {
        wait_video_line(MAX_IMAGE_HEIGHT);
    }

    // Flip main and shadow screens to display this animation step.
    layer2_flip_main_shadow_screen();

    // Flip frame_rects.
    flip_frame_rects();

    // Use the new shadow screen for the blit_background() calls below.
    blit_context.screen_base_page = GET_SCREEN_BASE_PAGE(SHADOW_SCREEN);

    // Restore the image background on the new shadow screen using the
    // frame_rects and num_frame_rects from the previous animation step.
    for (uint8_t i = 0; i < *num_frame_rects; i++)
    {
        blit_background(&frame_rects[i]);
    }
}
