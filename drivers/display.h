#ifndef DISPLAY_H
#define DISPLAY_H

#define FB_BASE     0xA8000000
#define FB_WIDTH    540     // visible pixels per row (panel resolution)
#define FB_HEIGHT   960
#define FB_STRIDE   1080    // physical scanline width in pixels (from memory size)
#define FB_BPP      3

void draw_char(unsigned char *fb,
               unsigned int x, unsigned int y, char c,
               unsigned char r, unsigned char g, unsigned char b,
               unsigned char bg_r, unsigned char bg_g, unsigned char bg_b);

void draw_string(unsigned char *fb,
                 unsigned int x, unsigned int y, const char *str,
                 unsigned char r, unsigned char g, unsigned char b,
                 unsigned char bg_r, unsigned char bg_g, unsigned char bg_b);

#endif