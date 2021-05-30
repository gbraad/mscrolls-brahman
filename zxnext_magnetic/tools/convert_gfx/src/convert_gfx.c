/*******************************************************************************
 * Stefan Bylund 2020
 *
 * Tool for extracting images and animated images from a Magnetic interpreter
 * graphics file (.gfx) and converting those images to separate NXA files (.nxa)
 * suitable for ZX Spectrum Next. This tool is mainly intended for the game
 * Wonderland.
 *
 * Since the animated image format in .gfx files are not formally specified,
 * the routines for extracting and parsing them are based on the extract() and
 * animate() functions from the Magnetic interpreter.
 *
 * This tool requires a little endian host environment.
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "nxa.h"

#define MAX_ANIM_FRAMES 200
#define MAX_ANIM_STEPS 400
#define MAX_POSITIONS_PER_ANIM_STEP 20

#define MAX_PICTURE_SIZE 0xC800
#define MAX_ANIM_FRAMES_SIZE 0x40000

#define MAX_IMAGE_HEIGHT 136
#define TRANSPARENCY_COLOR 0xE3
#define PAGE_SIZE 8192

#define MIN(a,b) (((int32_t)(a) < (int32_t)(b)) ? (int32_t)(a) : (int32_t)(b))
#define MAX(a,b) (((int32_t)(a) > (int32_t)(b)) ? (int32_t)(a) : (int32_t)(b))

struct pic_item
{
    char name[8];
    uint32_t offset;
    uint32_t size;
};

struct picture
{
    uint16_t width;
    uint16_t height;
    uint16_t wbytes;
    uint16_t plane_step;
    uint32_t data_size;
    uint8_t *data;
    uint8_t *mask;
};

struct position
{
    uint16_t id;
    int16_t number;
    int16_t x;
    int16_t y;
};

struct lookup
{
    int16_t flag;
    int16_t count;
};

struct anim_step
{
    uint8_t num_frame_positions;
    uint16_t frame_positions[MAX_POSITIONS_PER_ANIM_STEP];
};

static FILE *gfx_fp = NULL;

static struct pic_item *pic_items = NULL;
static uint16_t num_pic_items = 0;

static uint8_t *gfx_buf = NULL;
static uint8_t frame_buf[MAX_PICTURE_SIZE];

static struct picture anim_frame_table[MAX_ANIM_FRAMES];

static uint16_t pos_table_size = 0;
static uint16_t pos_table_count[MAX_POSITIONS_PER_ANIM_STEP];
static struct position pos_table[MAX_POSITIONS_PER_ANIM_STEP][MAX_ANIM_STEPS];

static uint8_t *command_table = NULL;
static int16_t command_index = -1;

static struct lookup anim_table[MAX_POSITIONS_PER_ANIM_STEP];

static uint16_t next_table = 1;

static int16_t pos_table_index = -1;
static int16_t pos_table_max = -1;

static struct position pos_array[MAX_POSITIONS_PER_ANIM_STEP];

static uint8_t nxa_palette[512];

static uint8_t nxa_image[256 * MAX_IMAGE_HEIGHT];

static nxa_frame_t nxa_frames[MAX_ANIM_FRAMES];

static struct position frame_positions[MAX_POSITIONS_PER_ANIM_STEP * MAX_ANIM_STEPS];
static nxa_frame_position_t nxa_frame_positions[MAX_POSITIONS_PER_ANIM_STEP * MAX_ANIM_STEPS];
static uint16_t num_frame_positions = 0;

static struct anim_step anim_steps[MAX_ANIM_STEPS];
static uint16_t anim_step_pos = 0;

static uint8_t nxa_frame_data[MAX_ANIM_FRAMES_SIZE];
static uint32_t nxa_frame_data_pos = 0;

// Read big endian word and return as little endian word.
static uint16_t read_w(uint8_t *ptr)
{
    return (uint16_t) (ptr[0] << 8 | ptr[1]);
}

// Read big endian long word and return as little endian long word.
static uint32_t read_l(uint8_t *ptr)
{
    return (uint32_t) ((uint32_t) ptr[0] << 24 | (uint32_t) ptr[1] << 16 | (uint32_t) ptr[2] << 8 | (uint32_t) ptr[3]);
}

// Read little endian word and return as little endian word.
static uint16_t read_w2(uint8_t *ptr)
{
    return (uint16_t) (ptr[1] << 8 | ptr[0]);
}

// Read mixed endian long word and return as little endian long word.
static uint32_t read_l2(uint8_t *ptr)
{
    return ((uint32_t) ptr[1] << 24 | (uint32_t) ptr[0] << 16 | (uint32_t) ptr[3] << 8 | (uint32_t) ptr[2]);
}

static uint32_t write_file_b(uint8_t value, FILE *nxa_file)
{
    return fwrite(&value, 1, 1, nxa_file);
}

static uint32_t write_file_w(uint16_t value, FILE *nxa_file)
{
    return fwrite(&value, 2, 1, nxa_file);
}

static uint32_t write_file_l(uint32_t value, FILE *nxa_file)
{
    return fwrite(&value, 4, 1, nxa_file);
}

static bool init_gfx(void)
{
    uint8_t header[6];
    uint16_t gfx_hdr_size;
    uint8_t *gfx_hdr;
    uint8_t *pic_ptr;
    uint16_t pic_num = 0;

    if (fread(header, 1, sizeof(header), gfx_fp) != sizeof(header))
    {
        return false;
    }

    if (read_l(header) != 0x4D615032) /* MaP2 */
    {
        return false;
    }

    gfx_hdr_size = read_w(header + 4);
    if (!(gfx_hdr = malloc(gfx_hdr_size)))
    {
        return false;
    }

    if (fread(gfx_hdr, 1, gfx_hdr_size, gfx_fp) != gfx_hdr_size)
    {
        free(gfx_hdr);
        return false;
    }

    if (!(pic_items = (struct pic_item *) malloc(gfx_hdr_size)))
    {
        free(gfx_hdr);
        return false;
    }
    num_pic_items = gfx_hdr_size / sizeof(struct pic_item);
    pic_ptr = gfx_hdr;

    for (uint16_t i = 0; i < num_pic_items; i++)
    {
        // Skip static version of animated image (uppercase filename).
        if (islower(pic_ptr[0]))
        {
            memset(pic_items[pic_num].name, 0, 8);
            strncpy(pic_items[pic_num].name, (char *) pic_ptr, 6);
            pic_items[pic_num].offset = read_l(pic_ptr + 8);
            pic_items[pic_num].size = read_l(pic_ptr + 12);
            pic_num++;
        }

        pic_ptr += sizeof(struct pic_item);
    }

    num_pic_items = pic_num;

    free(gfx_hdr);

    return true;
}

static void free_gfx(void)
{
    if (pic_items)
    {
        free(pic_items);
        pic_items = NULL;
    }

    if (gfx_buf)
    {
        free(gfx_buf);
        gfx_buf = NULL;
    }

    if (gfx_fp)
    {
        fclose(gfx_fp);
        gfx_fp = NULL;
    }
}

static void extract_frame(struct picture *pic)
{
    uint32_t ywb;
    uint32_t yw;
    uint32_t bit_x;
    uint32_t mask;
    uint32_t value;
    uint32_t values[4];

    if (pic->width * pic->height > MAX_PICTURE_SIZE)
    {
        fprintf(stderr, "Picture too large.\n");
        return;
    }

    for (uint16_t y = 0; y < pic->height; y++)
    {
        ywb = y * pic->wbytes;
        yw = y * pic->width;

        for (uint16_t x = 0; x < pic->width; x++)
        {
            if ((x % 8) == 0)
            {
                for (uint16_t i = 0; i < 4; i++)
                {
                    values[i] = pic->data[ywb + (x / 8) + (pic->plane_step * i)];
                }
            }

            bit_x = 7 - (x & 7);
            mask = 1 << bit_x;
            value = ((values[0] & mask) >> bit_x) << 0 |
                    ((values[1] & mask) >> bit_x) << 1 |
                    ((values[2] & mask) >> bit_x) << 2 |
                    ((values[3] & mask) >> bit_x) << 3;
            value &= 15;

            frame_buf[yw + x] = (uint8_t) value;
        }
    }
}

static uint16_t get_frame_position_index(struct position *pos)
{
    for (uint16_t i = 0; i < num_frame_positions; i++)
    {
        struct position *frame_position = &frame_positions[i];

        if  ((frame_position->number == pos->number) &&
             (frame_position->x == pos->x) &&
             (frame_position->y == pos->y))
        {
            return i;
        }
    }

    return 0xFFFF;
}

static uint8_t *extract(struct pic_item *pic,
                        uint16_t *w,
                        uint16_t *h,
                        uint16_t *pal,
                        uint8_t *is_anim,
                        uint8_t *num_anim_frames)
{
    struct picture main_pic;
    uint8_t *anim_data;

    pos_table_size = 0;
    num_frame_positions = 0;

    *is_anim = 0;
    *num_anim_frames = 0;

    if (gfx_buf)
    {
        free(gfx_buf);
    }

    gfx_buf = malloc(pic->size);
    if (!gfx_buf)
    {
        return NULL;
    }

    if (fseek(gfx_fp, pic->offset, SEEK_SET) != 0)
    {
        free(gfx_buf);
        gfx_buf = NULL;
        return NULL;
    }

    if (fread(gfx_buf, 1, pic->size, gfx_fp) != pic->size)
    {
        free(gfx_buf);
        gfx_buf = NULL;
        return NULL;
    }

    /* Get the main picture and palette */

    for (uint16_t i = 0; i < 16; i++)
    {
        pal[i] = read_w2(gfx_buf + 4 + (2 * i));
    }

    main_pic.data = gfx_buf + 48;
    main_pic.data_size = read_l2(gfx_buf + 38);
    main_pic.width = read_w2(gfx_buf + 42);
    main_pic.height = read_w2(gfx_buf + 44);
    main_pic.wbytes = (uint16_t) (main_pic.data_size / main_pic.height);
    main_pic.plane_step = (uint16_t) (main_pic.wbytes / 4);
    main_pic.mask = NULL;

    extract_frame(&main_pic);

    *w = main_pic.width;
    *h = main_pic.height;

    /* Check for an animation */
    anim_data = gfx_buf + 48 + main_pic.data_size;
    if ((anim_data[0] != 0xD0) || (anim_data[1] != 0x5E))
    {
        uint8_t *current;
        uint16_t frame_count;
        uint16_t command_count;

        *is_anim = 1;
        current = anim_data + 6;
        frame_count = read_w2(anim_data + 2);
        *num_anim_frames = (uint8_t) frame_count;
        if (frame_count > MAX_ANIM_FRAMES)
        {
            fprintf(stderr, "Animation frame array too short.\n");
            free(gfx_buf);
            return NULL;
        }

        /* Get the animation frames */
        for (uint16_t i = 0; i < frame_count; i++)
        {
            struct picture *anim_frame;
            uint16_t value1;
            uint16_t value2;

            anim_frame = &anim_frame_table[i];
            anim_frame->data = current + 10;
            anim_frame->data_size = read_l2(current);
            anim_frame->width = read_w2(current + 4);
            anim_frame->height = read_w2(current + 6);
            anim_frame->wbytes = (uint16_t) (anim_frame->data_size / anim_frame->height);
            anim_frame->plane_step = (uint16_t) (anim_frame->wbytes / 4);
            anim_frame->mask = NULL;

            current += anim_frame->data_size + 12;
            value1 = read_w2(current - 2);
            value2 = read_w2(current);

            /* Get the mask */
            if ((value1 == anim_frame->width) && (value2 == anim_frame->height))
            {
                uint16_t mask_size;

                anim_frame->mask = (uint8_t *) (current + 4);
                mask_size = read_w2(current + 2);
                current += mask_size + 6;
            }
        }

        /* Get the positioning tables */

        pos_table_size = read_w2(current - 2);
        if (pos_table_size > MAX_POSITIONS_PER_ANIM_STEP)
        {
            fprintf(stderr, "Animation position array too short.\n");
            free(gfx_buf);
            return NULL;
        }

        for (uint16_t i = 0; i < pos_table_size; i++)
        {
            pos_table_count[i] = read_w2(current + 2);
            current += 4;

            if (pos_table_count[i] > MAX_ANIM_STEPS)
            {
                fprintf(stderr, "Animation position array too short.\n");
                free(gfx_buf);
                return NULL;
            }

            for (uint16_t j = 0; j < pos_table_count[i]; j++)
            {
                struct position *pos = &pos_table[i][j];

                /* The frame positions are stored in two arrays; pos_table and frame_positions.
                 * The pos_table array is used by the animate() function and contains all positions.
                 * The frame_positions array is used when writing the positions to the nxa file and
                 * only contains the unique positions without any duplicates.
                 */

                pos->x = read_w2(current);
                pos->y = read_w2(current + 2);
                pos->number = read_w2(current + 4) - 1;

                if (pos->number != -1)
                {
                    // Optimization:
                    // Store only the unique frame positions in the frame_positions array.
                    // Duplicates have the same id as the existing ones.
                    uint16_t pos_index = get_frame_position_index(pos);
                    if (pos_index == 0xFFFF)
                    {
                        // Not previously added, set its id and add it.
                        pos->id = num_frame_positions;
                        frame_positions[num_frame_positions++] = *pos;
                    }
                    else
                    {
                        // Already added, set the id of the duplicate
                        // to the same id as the already added position.
                        pos->id = pos_index;
                    }
                }

                current += 8;
            }
        }

        /* Get the command sequence table */
        command_count = read_w2(current);
        command_table = current + 2;
        command_index = 0;
        (void) command_count;

        /* Reset animation variables before picture is animated */
        for (uint16_t i = 0; i < MAX_POSITIONS_PER_ANIM_STEP; i++)
        {
            anim_table[i].flag = -1;
            anim_table[i].count = -1;
        }
        pos_table_index = -1;
        pos_table_max = -1;
    }

    return frame_buf;
}

static uint8_t animate(char *name, struct position **positions, uint16_t *count)
{
    uint8_t got_anim = 0;

    if (gfx_buf == NULL)
    {
        return 0;
    }

    if ((pos_table_size == 0) || (command_index < 0))
    {
        return 0;
    }

    *positions = NULL;
    *count = 0;

    while (!got_anim)
    {
        if (pos_table_max >= 0)
        {
            if (pos_table_index < pos_table_max)
            {
                for (uint16_t i = 0; i < pos_table_size; i++)
                {
                    if (anim_table[i].flag > -1)
                    {
                        if (*count >= MAX_POSITIONS_PER_ANIM_STEP)
                        {
                            fprintf(stderr, "Returned animation array too short.\n");
                            return 0;
                        }

                        pos_array[*count] = pos_table[i][anim_table[i].flag];
                        (*count)++;

                        if (anim_table[i].flag < (pos_table_count[i] - 1))
                        {
                            anim_table[i].flag++;
                        }

                        if (anim_table[i].count > 0)
                        {
                            anim_table[i].count--;
                        }
                        else
                        {
                            anim_table[i].flag = -1;
                        }
                    }
                }

                if (*count > 0)
                {
                    *positions = pos_array;
                    got_anim = 1;
                }

                pos_table_index++;
            }
        }

        if (!got_anim)
        {
            uint8_t command;
            uint16_t ttable;

            command = command_table[command_index];
            command_index++;

            pos_table_max = -1;
            pos_table_index = -1;

            switch (command)
            {
            case 0x00: // AN_OP_END (not used)
                command_index = -1;
                return 0;
            case 0x01: // AN_OP_RUN_SEQUENCE (used in all anims a lot)
                // Load animation frame table
                ttable = command_table[command_index];
                command_index++;

                if (ttable - 1 >= MAX_POSITIONS_PER_ANIM_STEP)
                {
                    fprintf(stderr, "Animation table too short.\n");
                    return 0;
                }

                anim_table[ttable - 1].flag = (int16_t) (command_table[command_index] - 1);
                command_index++;
                anim_table[ttable - 1].count = (int16_t) (command_table[command_index] - 1);
                command_index++;

                /* Workaround for Wonderland "catter" animation */
                if (strcmp(name, "catter") == 0)
                {
                    if (command_index == 96)
                    {
                        anim_table[ttable - 1].count = 9;
                    }
                    if (command_index == 108)
                    {
                        anim_table[ttable - 1].flag = -1;
                    }
                    if (command_index == 156)
                    {
                        anim_table[ttable - 1].flag = -1;
                    }
                }
                break;
            case 0x02: // AN_OP_WAIT_FOR (used in all anims a lot)
                // Animate
                pos_table_max = command_table[command_index];
                pos_table_index = 0;
                command_index++;
                break;
            case 0x03: // AN_OP_GOTO (used once in all anims except cooksb and rbrige)
                // Stop/repeat
                command_index = -1;
                return 0;
            case 0x04: // AN_OP_RANDOM_WAIT (only used in cooksb and rbrige but not handled)
                // Unknown command, not parsed
                command_index += 3;
                return 0;
            case 0x05: // AN_OP_GOTO_IF_RANDOM (only used in image outpal)
                ttable = next_table;
                command_index++;

                anim_table[ttable - 1].flag = 0;
                anim_table[ttable - 1].count = command_table[command_index];

                pos_table_max = command_table[command_index];
                pos_table_index = 0;
                command_index++;
                command_index++;
                next_table++;
                break;
            default:
                fprintf(stderr, "Unknown animation command.\n");
                command_index = -1;
                return 0;
            }
        }
    }

    return got_anim;
}

static uint8_t *get_anim_frame(int16_t number, uint16_t *width, uint16_t *height, uint8_t **mask)
{
    if ((number >= 0) && (number < MAX_ANIM_FRAMES))
    {
        struct picture *anim_frame = &anim_frame_table[number];
        extract_frame(anim_frame);
        *width = anim_frame->width;
        *height = anim_frame->height;
        *mask = anim_frame->mask;
        return frame_buf;
    }

    return NULL;
}

static void create_nxa_palette(uint16_t *palette)
{
    memset(nxa_palette, 0, sizeof(nxa_palette));

    // Convert from 00000RRR,0GGG0BBB to 0000000B,RRRGGGBB.

    for (uint16_t i = 0; i < 16; i++)
    {
        uint16_t color = palette[i];

        uint16_t r = (color & 0x0700) >> 8;
        uint16_t g = (color & 0x0070) >> 4;
        uint16_t b = (color & 0x0007) >> 0;

        uint16_t rgb333 = (r << 6) | (g << 3) | (b << 0);
        uint8_t rgb332 = (uint8_t) (rgb333 >> 1);
        uint8_t b1 = (uint8_t) (rgb333 & 0x01);

        nxa_palette[i * 2 + 0] = rgb332;
        nxa_palette[i * 2 + 1] = b1;
    }

    // Set the transparency color in the palette.
    // TRANSPARENCY_COLOR is both the palette index of the transparency color
    // and an RGB332 color value.
    // The corresponding RGB333 color value has its lowest blue bit as a bitwise
    // OR between the two blue bits in the RGB332 color.

    nxa_palette[TRANSPARENCY_COLOR * 2 + 0] = TRANSPARENCY_COLOR;
    nxa_palette[TRANSPARENCY_COLOR * 2 + 1] = ((TRANSPARENCY_COLOR & 0x02) >> 1) | (TRANSPARENCY_COLOR & 0x01);

    if ((nxa_palette[0] != 0) && (nxa_palette[1] != 0))
    {
        printf("Warning: First color in palette is not black.\n");
    }
}

static void create_nxa_image(char *name,
                             uint8_t *picture,
                             uint16_t picture_width,
                             uint16_t picture_height,
                             uint8_t *image_x,
                             uint8_t *image_y,
                             uint16_t *image_width,
                             uint8_t *image_height)
{
    uint16_t width_clipped;
    uint16_t height_clipped;
    uint16_t src_x_rest;
    uint16_t dst_x_margin;
    uint16_t dst_x;
    uint16_t dst_y;
    uint8_t *src_ptr;
    uint8_t *dst_ptr;

    memset(nxa_image, 0, sizeof(nxa_image));

    width_clipped = (uint16_t) MIN(picture_width, 256);
    height_clipped = (uint16_t) MIN(picture_height, MAX_IMAGE_HEIGHT - 2);

    src_x_rest = picture_width - width_clipped;
    dst_x_margin = 256 - width_clipped;
    dst_x = dst_x_margin / 2; // horizontally centered
    dst_y = MAX_IMAGE_HEIGHT - 2 - height_clipped;

    src_ptr = picture;
    dst_ptr = nxa_image + dst_x + dst_y * 256;

    // Adjust specific images.
    if (!strcmp(name, "rbrige"))
    {
        src_ptr = picture + 1;
    }
    else if (!strcmp(name, "tunnel"))
    {
        src_ptr = picture + 5 * picture_width;
    }

    for (uint16_t y = 0; y < height_clipped; y++)
    {
         for (uint16_t x = 0; x < width_clipped; x++)
         {
             *dst_ptr++ = *src_ptr++;
         }

         src_ptr += src_x_rest;
         dst_ptr += dst_x_margin;
    }

    *image_x = (uint8_t) dst_x;
    *image_y = (uint8_t) dst_y;
    *image_width = width_clipped;
    *image_height = (uint8_t) height_clipped;
}

static void add_frame_transparency(uint8_t *frame, uint16_t width, uint16_t height, uint8_t *mask)
{
    /*
     * The mask has one bit for every pixel in the frame. A zero bit means the
     * pixel is opaque, while a one bit means the pixel is transparent.
     * The mask is made up of lines of 16-bit words, so the mask width in bytes
     * is always even.
     */

    uint16_t mask_width = (((width - 1) / 8) + 2) & (~1);

    for (uint16_t y = 0; y < height; y++)
    {
         for (uint16_t x = 0; x < width; x++)
         {
             if ((mask[(y * mask_width) + (x / 8)] & (0x80 >> (x % 8))) != 0)
             {
                 frame[(y * width) + x] = TRANSPARENCY_COLOR;
             }
         }
    }
}

static void adjust_nxa_frame_position(char *name, nxa_frame_position_t *nxa_frame_position)
{
    // Adjust specific images.
    if (!strcmp(name, "rbrige"))
    {
        nxa_frame_position->x -= 1;
    }
    else if (!strcmp(name, "tunnel"))
    {
        nxa_frame_position->y -= 5;
    }
}

static uint8_t is_anim_repeating(char *name)
{
    if (!strcmp(name, "beaver"))
    {
        return 1;
    }
    else if (!strcmp(name, "enthal"))
    {
        return 1;
    }
    else if (!strcmp(name, "lounge"))
    {
        return 1;
    }
    else if (!strcmp(name, "music"))
    {
        return 1;
    }
    else if (!strcmp(name, "onbrge"))
    {
        return 1;
    }
    else if (!strcmp(name, "palkit"))
    {
        return 1;
    }
    else if (!strcmp(name, "tunnel"))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static int16_t trim_anim_steps_end(void)
{
    uint16_t new_anim_step_pos = anim_step_pos;
    int16_t num_trimmed;

    // An empty last animation step can be removed.
    if ((anim_step_pos > 2) && (anim_steps[anim_step_pos].num_frame_positions == 0))
    {
        anim_step_pos--;
    }

    // Trim identical animation steps at the end.
    for (int16_t i = anim_step_pos; i > 0; i--)
    {
        if (anim_steps[i].num_frame_positions == anim_steps[i - 1].num_frame_positions)
        {
            bool equal = true;

            for (uint16_t j = 0; j < anim_steps[i].num_frame_positions; j++)
            {
                if (anim_steps[i].frame_positions[j] != anim_steps[i - 1].frame_positions[j])
                {
                    equal = false;
                    break;
                }
            }

            if (equal)
            {
                new_anim_step_pos--;
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    // Leave one identical...
    num_trimmed = anim_step_pos - new_anim_step_pos;
    if (num_trimmed > 0)
    {
        new_anim_step_pos++;
        num_trimmed--;
    }
    anim_step_pos = new_anim_step_pos;

    return num_trimmed;
}

static uint16_t anim_steps_size(void)
{
    uint16_t size = 0;

    for (uint16_t i = 0; i < anim_step_pos + 1; i++)
    {
        size += 1 + anim_steps[i].num_frame_positions * 2;
    }

    return size;
}

static void shift_down_anim_steps(uint16_t removed_index)
{
    for (uint16_t i = 0; i < anim_step_pos + 1; i++)
    {
        for (uint16_t j = 0; j < anim_steps[i].num_frame_positions; j++)
        {
            if (anim_steps[i].frame_positions[j] > removed_index)
            {
                anim_steps[i].frame_positions[j] -= 1;
            }
        }
    }
}

static void remove_unused_frame_position(uint16_t index)
{
    if (index < num_frame_positions - 1)
    {
        memmove(&nxa_frame_positions[index], &nxa_frame_positions[index + 1],
                (num_frame_positions - (index + 1)) * sizeof(nxa_frame_position_t));
    }

    num_frame_positions--;
}

static bool is_frame_position_used(uint16_t index)
{
    bool used = false;

    for (uint16_t i = 0; i < anim_step_pos + 1; i++)
    {
        for (uint16_t j = 0; j < anim_steps[i].num_frame_positions; j++)
        {
            if (index == anim_steps[i].frame_positions[j])
            {
                used = true;
                break;
            }
        }
    }

    return used;
}

static uint16_t remove_unused_frame_positions(void)
{
    uint16_t num_removed = 0;

    for (int32_t i = num_frame_positions - 1; i >= 0; i--)
    {
        if (!is_frame_position_used(i))
        {
            remove_unused_frame_position(i);
            shift_down_anim_steps(i);
            num_removed++;
        }
    }

    return num_removed;
}

static void write_nxa_image_header(nxa_image_header_t *nxa_image_header, FILE *nxa_file)
{
    write_file_b(nxa_image_header->x, nxa_file);
    write_file_b(nxa_image_header->y, nxa_file);
    write_file_w(nxa_image_header->width, nxa_file);
    write_file_b(nxa_image_header->height, nxa_file);
    write_file_w(nxa_image_header->size, nxa_file);
    write_file_b(nxa_image_header->animated, nxa_file);
}

static void write_nxa_palette(FILE *nxa_file)
{
    fwrite(nxa_palette, 1, sizeof(nxa_palette), nxa_file);
}

static void write_nxa_image(FILE *nxa_file)
{
    fwrite(nxa_image, 1, sizeof(nxa_image), nxa_file);
}

static void write_nxa_anim_header(nxa_anim_header_t *nxa_anim_header, FILE *nxa_file)
{
    write_file_b(nxa_anim_header->interval, nxa_file);
    write_file_b(nxa_anim_header->repeating, nxa_file);
    write_file_b(nxa_anim_header->transparency_color, nxa_file);
    write_file_b(nxa_anim_header->num_frames, nxa_file);
    write_file_w(nxa_anim_header->num_positions, nxa_file);
    write_file_w(nxa_anim_header->num_steps, nxa_file);
    write_file_w(nxa_anim_header->steps_size, nxa_file);
    write_file_l(nxa_anim_header->frame_data_size, nxa_file);
}

static void write_nxa_frames(uint8_t num_frames, FILE *nxa_file)
{
    for (uint16_t i = 0; i < num_frames; i++)
    {
        nxa_frame_t *frame = &nxa_frames[i];
        write_file_b(frame->number, nxa_file);
        write_file_w(frame->width, nxa_file);
        write_file_b(frame->height, nxa_file);
        write_file_b(frame->transparent, nxa_file);
        write_file_b(frame->page, nxa_file);
        write_file_w(frame->offset, nxa_file);
        write_file_w(frame->size, nxa_file);
    }
}

static void write_nxa_frame_positions(FILE *nxa_file)
{
    for (uint16_t i = 0; i < num_frame_positions; i++)
    {
        nxa_frame_position_t *frame_position = &nxa_frame_positions[i];
        write_file_b(frame_position->frame_number, nxa_file);
        write_file_w(frame_position->x, nxa_file);
        write_file_w(frame_position->y, nxa_file);
    }
}

static void write_nxa_anim_steps(FILE *nxa_file)
{
    for (uint16_t i = 0; i < anim_step_pos + 1; i++)
    {
        struct anim_step *anim_step = &anim_steps[i];
        write_file_b(anim_step->num_frame_positions, nxa_file);
        for (uint16_t j = 0; j < anim_step->num_frame_positions; j++)
        {
            write_file_w(anim_step->frame_positions[j], nxa_file);
        }
    }
}

static void write_nxa_frame_data(FILE *nxa_file)
{
    fwrite(nxa_frame_data, 1, nxa_frame_data_pos + 1, nxa_file);
}

static void write_nxa_file(struct pic_item *pic)
{
    uint8_t *picture;
    uint16_t picture_width;
    uint16_t picture_height;
    uint16_t palette[16];
    uint8_t is_anim = 0;
    uint8_t num_anim_frames = 0;

    uint8_t image_x;
    uint8_t image_y;
    uint16_t image_width;
    uint8_t image_height;
    nxa_image_header_t nxa_image_header;
    char nxa_filename[16];
    FILE *nxa_file;

    // Extract a picture from the .gfx file.
    picture = extract(pic, &picture_width, &picture_height, palette, &is_anim, &num_anim_frames);
    if (picture == NULL)
    {
        fprintf(stderr, "Error extracting picture %s.\n", pic->name);
        return;
    }

    create_nxa_palette(palette);
    create_nxa_image(pic->name, picture, picture_width, picture_height,
            &image_x, &image_y, &image_width, &image_height);

    nxa_image_header.x = image_x;
    nxa_image_header.y = image_y;
    nxa_image_header.width = image_width;
    nxa_image_header.height = image_height;
    nxa_image_header.size = 256 * MAX_IMAGE_HEIGHT;
    nxa_image_header.animated = is_anim;

    sprintf(nxa_filename, "%s.nxa", pic->name);
    nxa_file = fopen(nxa_filename, "wb");
    if (nxa_file == NULL)
    {
        fprintf(stderr, "Error creating NXA file %s.\n", nxa_filename);
        return;
    }

    write_nxa_image_header(&nxa_image_header, nxa_file);
    write_nxa_palette(nxa_file);
    write_nxa_image(nxa_file);

    if (is_anim)
    {
        struct position *positions;
        uint16_t num_positions;
        nxa_anim_header_t nxa_anim_header;

        nxa_frame_data_pos = 0;
        anim_step_pos = 0;

        memset(nxa_frames, 0, sizeof(nxa_frames));
        memset(nxa_frame_data, 0, sizeof(nxa_frame_data));
        memset(nxa_frame_positions, 0, sizeof(nxa_frame_positions));
        memset(anim_steps, 0, sizeof(anim_steps));

        // Convert animation frames.
        for (uint16_t i = 0; i < num_anim_frames; i++)
        {
            uint8_t *frame_data;
            uint16_t frame_width;
            uint16_t frame_height;
            uint8_t *frame_mask = NULL;
            uint16_t frame_size;
            nxa_frame_t *nxa_frame;

            frame_data = get_anim_frame(i, &frame_width, &frame_height, &frame_mask);
            if (frame_data == NULL)
            {
                fprintf(stderr, "Missing frame data for frame %u in picture %s.\n", i, pic->name);
                fclose(nxa_file);
                return;
            }
            if (frame_mask != NULL)
            {
                add_frame_transparency(frame_data, frame_width, frame_height, frame_mask);
            }
            frame_size = frame_width * frame_height;

            nxa_frame = &nxa_frames[i];
            nxa_frame->number = (uint8_t) i;
            nxa_frame->width = frame_width;
            nxa_frame->height = (uint8_t) frame_height;
            nxa_frame->transparent = (frame_mask != NULL);
            nxa_frame->page = (uint8_t) (nxa_frame_data_pos / PAGE_SIZE);
            nxa_frame->offset = (uint16_t) (nxa_frame_data_pos % PAGE_SIZE);
            nxa_frame->size = frame_size;

            memcpy(nxa_frame_data + nxa_frame_data_pos, frame_data, frame_size);
            nxa_frame_data_pos += frame_size;
        }

        // Convert animation frame positions.
        for (uint16_t i = 0; i < num_frame_positions; i++)
        {
            nxa_frame_position_t *nxa_frame_position = &nxa_frame_positions[i];
            nxa_frame_position->frame_number = (uint8_t) frame_positions[i].number;
            nxa_frame_position->x = frame_positions[i].x;
            nxa_frame_position->y = frame_positions[i].y;

            adjust_nxa_frame_position(pic->name, nxa_frame_position);
        }

        // Convert animation steps.
        while (animate(pic->name, &positions, &num_positions))
        {
            uint8_t num_actual_positions = 0;

            for (uint16_t i = 0; i < num_positions; i++)
            {
                struct position *position;
                uint8_t *frame_data;
                uint16_t frame_width;
                uint16_t frame_height;
                uint8_t *frame_mask = NULL;

                position = &positions[i];
                frame_data = get_anim_frame(position->number, &frame_width, &frame_height, &frame_mask);

                if (frame_data != NULL)
                {
                    int32_t x_min = MAX(0, position->x);
                    int32_t y_min = MAX(0, position->y);
                    int32_t x_max = MIN(picture_width, position->x + frame_width) - 1;
                    int32_t y_max = MIN(picture_height, position->y + frame_height) - 1;

                    // Skip frame positions that are outside of main image.
                    if (x_max < x_min || y_max < y_min)
                    {
                        continue;
                    }

                    anim_steps[anim_step_pos].frame_positions[num_actual_positions] = position->id;
                    num_actual_positions++;
                }
            }

            anim_steps[anim_step_pos].num_frame_positions = num_actual_positions;
            anim_step_pos++;
        }

        // Optimize animation steps and animation frame positions.
        trim_anim_steps_end();
        remove_unused_frame_positions();

        nxa_anim_header.interval = 5;
        nxa_anim_header.repeating = is_anim_repeating(pic->name);
        nxa_anim_header.transparency_color = TRANSPARENCY_COLOR;
        nxa_anim_header.num_frames = num_anim_frames;
        nxa_anim_header.num_positions = num_frame_positions;
        nxa_anim_header.num_steps = anim_step_pos;
        nxa_anim_header.steps_size = anim_steps_size();
        nxa_anim_header.frame_data_size = nxa_frame_data_pos + 1;

        write_nxa_anim_header(&nxa_anim_header, nxa_file);
        write_nxa_frames(num_anim_frames, nxa_file);
        write_nxa_frame_positions(nxa_file);
        write_nxa_anim_steps(nxa_file);
        write_nxa_frame_data(nxa_file);

        printf("Image: %s, frames: %u, positions: %u, steps: %u, step size: %u, frame data size: %u, repeating: %u\n",
            pic->name,
            nxa_anim_header.num_frames,
            nxa_anim_header.num_positions,
            nxa_anim_header.num_steps,
            nxa_anim_header.steps_size,
            nxa_anim_header.frame_data_size,
            nxa_anim_header.repeating);
    }

    fclose(nxa_file);
}

static void write_nxa_files(void)
{
    for (uint16_t i = 0; i < num_pic_items; i++)
    {
        write_nxa_file(&pic_items[i]);
    }
}

int main(int argc, char *argv[])
{
    char *filename = (argc > 1) ? argv[1] : "wonder.gfx";

    if (!(gfx_fp = fopen(filename, "rb")))
    {
        fprintf(stderr, "Cannot open file %s.\n", filename);
        return 1;
    }

    if (!init_gfx())
    {
        fprintf(stderr, "Error parsing header of file %s.\n", filename);
        fclose(gfx_fp);
        return 1;
    }

    write_nxa_files();
    free_gfx();
    return 0;
}
