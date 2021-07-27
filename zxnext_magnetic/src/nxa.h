/*******************************************************************************
 * Stefan Bylund 2020
 *
 * NXA animated image file format definition.
 *
 * An NXA file describes an animated layer 2 image for the ZX Spectrum Next.
 *
 * A program that displays an NXA image should read the image header, read and
 * set the palette, and read and display the image. Note that the palette and
 * image data part represents a valid NXI image.
 *
 * If the image is animated, the program should read and display the animation
 * frame(s) in each animation step with the specified time interval. If the
 * animation is repeating, the animation steps are repeated forever otherwise
 * the main image should be diplayed again.
 *
 * NOTE: Structs are always packed in z88dk (i.e. no padding) and little-endian
 * byte order is used. This must be considered when writing a host tool for
 * creating NXA files.
 *
 * Layout of an NXA animated image file:
 *
 *    |-----------------------------------|
 *    | Image header                      |
 *    | * image x                         |
 *    | * image y                         |
 *    | * image width                     |
 *    | * image height                    |
 *    | * image size                      |
 *    | * is image animated               |
 *    |-----------------------------------|
 *    | Palette                           |
 *    | * 256 palette colors              |
 *    |-----------------------------------|
 *    | Image data                        |
 *    | * pixel data                      |
 *    |-----------------------------------|
 *    | Animation header                  |
 *    | * animation interval              |
 *    | * is animation repeating          |
 *    | * transparency color              |
 *    | * num animation frames            |
 *    | * num animation frame positions   |
 *    | * num animation steps             |
 *    | * animation steps size            |
 *    | * total animation frame data size |
 *    |===================================|
 *    | Animation frame 0                 |
 *    | * animation frame number          |
 *    | * animation frame width           |
 *    | * animation frame height          |
 *    | * is animation frame transparent  |
 *    | * page number                     |
 *    | * offset in page                  |
 *    | * animation frame data size       |
 *    |-----------------------------------|
 *    | Animation frame 1                 |
 *    |-----------------------------------|
 *    | ...                               |
 *    |-----------------------------------|
 *    | Animation frame N                 |
 *    |===================================|
 *    | Animation frame position 0        |
 *    | * animation frame number          |
 *    | * x                               |
 *    | * y                               |
 *    |-----------------------------------|
 *    | Animation frame position 1        |
 *    |-----------------------------------|
 *    | ...                               |
 *    |-----------------------------------|
 *    | Animation frame position P        |
 *    |===================================|
 *    | Animation step 0                  |
 *    | * num animation frame positions   |
 *    | * animation frame position index  |
 *    | * animation frame position index  |
 *    | ...                               |
 *    | * animation frame position index  |
 *    |-----------------------------------|
 *    | Animation step 1                  |
 *    |-----------------------------------|
 *    | ...                               |
 *    |-----------------------------------|
 *    | Animation step S                  |
 *    |===================================|    |------
 *    | Animation frame data 0            |    |Page 0
 *    | * pixel data                      |    |------
 *    |-----------------------------------|    |Page 1
 *    | Animation frame data 1            |    |------
 *    |-----------------------------------|    |
 *    | ...                               |    |...
 *    |-----------------------------------|    |
 *    | Animation frame data N            |    |Page M
 *    |-----------------------------------|    |------
 *
 ******************************************************************************/

#ifndef _NXA_H
#define _NXA_H

#include <stdint.h>

/*
 * NXA animated image file header.
 *
 * Specifies the position (in screen coordinates) and the width and height of
 * the image, the size of the static image data and whether the image is
 * animated (1) or not (0).
 *
 * Followed by the palette, the image data and an optional animation header.
 */
typedef struct nxa_image_header
{
    uint8_t x;
    uint8_t y;
    uint16_t width;
    uint8_t height;
    uint16_t size;
    uint8_t animated;
} nxa_image_header_t;

/*
 * Palette for the image and its animation frames.
 *
 * Contains 256 RGB333 colors of the ZX Spectrum Next form 0000000B,RRRGGGBB.
 */
typedef struct nxa_palette
{
    uint16_t palette[256];
} nxa_palette_t;

/*
 * Image pixel data in layer 2 format, where each pixel is an 8-bit index into
 * the palette.
 */
typedef struct nxa_image_data
{
    uint8_t *data;
} nxa_image_data_t;

/*
 * NXA animation header.
 *
 * Followed by <num_frames> animation frames, <num_positions> animation frame
 * positions, <num_steps> animation steps and finally the animation frame data
 * whose total size is <frame_data_size>. Since the animation steps can be of
 * variable size, their total size in bytes is specified in <steps_size>.
 *
 * The interval specifies the number of 50 Hz display frames between each step
 * of the animation. For example, if the intended animation rate is 10 frames
 * per second (fps), the interval is 5.
 *
 * If the animation is repeating (1), the animation steps are repeated forever,
 * otherwise (0) the main image should be diplayed again when the animation is
 * over.
 *
 * If an animation frame contains transparent pixels, the specified transparency
 * color palette index is used.
 */
typedef struct nxa_anim_header
{
    uint8_t interval;
    uint8_t repeating;
    uint8_t transparency_color;
    uint8_t num_frames;
    uint16_t num_positions;
    uint16_t num_steps;
    uint16_t steps_size;
    uint32_t frame_data_size;
} nxa_anim_header_t;

/*
 * Animation frame.
 *
 * Specifies the width and height of an animation frame, the number of the
 * animation frame, whether it contains transparent pixels or not, and the
 * relative 8 KB page where its data is stored, the offset within that page
 * and the data size. If the frame data is too large to fit within the page,
 * it will spill over into the consecutive page(s).
 *
 * The animation frames are stored consecutively in an array in frame number
 * order, i.e. first frame 0, then frame 1 etc.
 */
typedef struct nxa_frame
{
    uint8_t number;
    uint16_t width;
    uint8_t height;
    uint8_t transparent;
    uint8_t page;
    uint16_t offset;
    uint16_t size;
} nxa_frame_t;

/*
 * Positioning of an animation frame.
 *
 * Specifies the x and y coordinates of the specified animation frame relative
 * to the origin of the main image. The coordinates can be negative for a frame
 * that is partially outside of the main image.
 *
 * The frame positions are stored consecutively in an array.
 */
typedef struct nxa_frame_position
{
    uint8_t frame_number;
    int16_t x;
    int16_t y;
} nxa_frame_position_t;

/*
 * Animation step describing the frame positions in a step where each frame
 * position describes the (x,y) coordinates of a specified animation frame.
 *
 * The animation steps are stored consecutively in display order, i.e. first
 * step 0 then step 1 etc. An animation step specifies how many frame positions
 * should be displayed (0 - 255) and an array of the frame position indexes used
 * to lookup the corresponding frame positions in the frame positions array.
 *
 * If the number of frame positions in an animation step is 0 it means that
 * no new animation frames for this step should be drawn after clearing the
 * animation frame(s) from the previous animation step. This effectively
 * pauses the animation and displays the main image.
 */
typedef struct nxa_anim_step
{
    uint8_t num_frame_positions;
    uint16_t frame_positions[1]; // variable size, can be empty
} nxa_anim_step_t;

/*
 * Animation frame pixel data in layer 2 format, where each pixel is an 8-bit
 * index into the palette.
 *
 * The animation frames are stored contiguosly in frame number order, i.e. first
 * frame 0, then frame 1 etc.
 */
typedef struct nxa_frame_data
{
    uint8_t *data;
} nxa_frame_data_t;

#endif
