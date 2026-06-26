#include <stdint.h>
#include "font.h"

#define FB_BASE     0xA8000000
#define FB_WIDTH    540     // visible pixels per row (panel resolution)
#define FB_HEIGHT   960
#define FB_STRIDE   1080    // physical scanline width in pixels (from memory size)
#define FB_BPP      3

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

void draw_char(unsigned char *fb,
               unsigned int x, unsigned int y, char c,
               unsigned char r, unsigned char g, unsigned char b,
               unsigned char bg_r, unsigned char bg_g, unsigned char bg_b) {
    if (c < 32 || c > 126) return;
    uint8_t charIndex = (c - 32);
    for (unsigned int row = 0; row < 5; row++) {
        for (unsigned int col = 0; col < 7; col++) {
            uint8_t fontByte = fontdata[charIndex][col];
            if (fontByte & (1 << (4 - row)))
                draw_pixel(fb, x + row, y + col, r, g, b);
            else
                draw_pixel(fb, x + row, y + col, bg_r, bg_g, bg_b);
        }
    }
}

void draw_string(unsigned char *fb,
                 unsigned int x, unsigned int y, const char *str,
                 unsigned char r, unsigned char g, unsigned char b,
                 unsigned char bg_r, unsigned char bg_g, unsigned char bg_b) {
    while (*str) {
        draw_char(fb, x, y, *str++, r, g, b, bg_r, bg_g, bg_b);
        x += 6;
        if (x > FB_WIDTH - 6) {
            y += 8;
            x = 0;
        }
    }
}

#define WDT_BASE        0x0B017000
#define WDT_RST         (*(volatile unsigned int *)(WDT_BASE + 0x04))
#define WDT_EN          (*(volatile unsigned int *)(WDT_BASE + 0x08))

static void watchdog_disable(void) {
    WDT_EN = 0;
}

static void watchdog_kick(void) {
    WDT_RST = 1;
}

#define QTIMER_FREQ_HZ  19200000ULL

static inline unsigned long long read_cntpct(void) {
    unsigned long long val;
    __asm__ volatile ("mrs %0, cntpct_el0" : "=r"(val));
    return val;
}

unsigned int hal_gettick_ms(void) {
    return (unsigned int)(read_cntpct() / (QTIMER_FREQ_HZ / 1000));
}

void hal_delay_ms(unsigned int ms) {
    unsigned long long start = read_cntpct();
    unsigned long long ticks = (unsigned long long)ms * (QTIMER_FREQ_HZ / 1000);
    while ((read_cntpct() - start) < ticks);
}

void main(void *dtb, void *kernel) {
	watchdog_disable();

    unsigned char *fb = (unsigned char *)FB_BASE;

    // clear to white
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT * FB_BPP; i++)
        fb[i] = 0x00;

	draw_string(fb, 10, 10, "Hello World!", 255, 255, 255, 0, 0, 0);

	// fill_rect(fb,   0,   0, 180, 320, 255,   0,   0); // red
    // fill_rect(fb, 180,   0, 180, 320,   0, 255,   0); // green
    // fill_rect(fb, 360,   0, 180, 320,   0,   0, 255); // blue
    // fill_rect(fb,   0, 320, 180, 320, 255, 255,   0); // yellow
    // fill_rect(fb, 180, 320, 180, 320,   0, 255, 255); // cyan
    // fill_rect(fb, 360, 320, 180, 320, 255,   0, 255); // magenta
    // fill_rect(fb,   0, 640, 180, 320, 255, 255, 255); // white
    // fill_rect(fb, 180, 640, 180, 320, 128, 128, 128); // gray
    // fill_rect(fb, 360, 640, 180, 320,  64,  64,  64); // dark gray

    hal_delay_ms(1000);

    // clear once
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT * FB_BPP; i++)
        fb[i] = 0x00;

    uint8_t count = 0;

    while(1) {
        if (count % 2 == 0) {
            draw_string(fb, 10, 50, "tick even!", 255, 255, 255, 0, 0, 0);
        } else {
            draw_string(fb, 10, 50, "tick odd!", 255, 255, 255, 0, 0, 0);
        }
        count++;
        hal_delay_ms(1000);
    }
}