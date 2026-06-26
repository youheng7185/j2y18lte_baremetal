#include <stdint.h>
#include "font.h"
#include "display.h"

__attribute__((noinline))
static void fill_rect(unsigned char *fb,
                      unsigned int x,  unsigned int y,
                      unsigned int w,  unsigned int h,
                      unsigned char r, unsigned char g, unsigned char b) {
    x = x + 286;
	unsigned char *row_ptr = fb + y * (FB_WIDTH * FB_BPP);
    for (unsigned int row = 0; row < h; row++) {
        unsigned char *px = row_ptr + x * FB_BPP;
        for (unsigned int col = 0; col < w; col++) {
            px[0] = r;
            px[1] = g;
            px[2] = b;
            px += FB_BPP;
        }
        row_ptr += FB_WIDTH * FB_BPP;
    }
}

__attribute__((noinline))
static void draw_pixel(unsigned char *fb,
                       unsigned int x, unsigned int y,
                       unsigned char r, unsigned char g, unsigned char b) {
    x = x + 286;
    unsigned char *px = fb + y * (FB_WIDTH * FB_BPP) + x * FB_BPP;
    px[0] = r;
    px[1] = g;
    px[2] = b;
}

#define CHAR_SCALE 3

void draw_char(unsigned char *fb,
               unsigned int x, unsigned int y, char c,
               unsigned char r, unsigned char g, unsigned char b,
               unsigned char bg_r, unsigned char bg_g, unsigned char bg_b)
{
    if (c < 32 || c > 126)
        return;

    uint8_t charIndex = c - 32;

    for (unsigned int row = 0; row < 5; row++) {
        for (unsigned int col = 0; col < 7; col++) {

            uint8_t fontByte = fontdata[charIndex][col];
            int set = fontByte & (1 << (4 - row));

            /* Expand one font pixel into a CHAR_SCALE x CHAR_SCALE block */
            for (unsigned int dy = 0; dy < CHAR_SCALE; dy++) {
                for (unsigned int dx = 0; dx < CHAR_SCALE; dx++) {

                    if (set)
                        draw_pixel(fb,
                                   x + row * CHAR_SCALE + dx,
                                   y + col * CHAR_SCALE + dy,
                                   r, g, b);
                    else
                        draw_pixel(fb,
                                   x + row * CHAR_SCALE + dx,
                                   y + col * CHAR_SCALE + dy,
                                   bg_r, bg_g, bg_b);
                }
            }
        }
    }
}

void draw_string(unsigned char *fb,
                 unsigned int x, unsigned int y, const char *str,
                 unsigned char r, unsigned char g, unsigned char b,
                 unsigned char bg_r, unsigned char bg_g, unsigned char bg_b)
{
    const unsigned int char_width  = 6 * CHAR_SCALE; // 5 pixels + 1 spacing
    const unsigned int line_height = 8 * CHAR_SCALE; // 7 pixels + 1 spacing

    while (*str) {
        draw_char(fb, x, y, *str++,
                  r, g, b,
                  bg_r, bg_g, bg_b);

        x += char_width;

        if (x > FB_WIDTH - char_width) {
            y += line_height;
            x = 0;
        }
    }
}