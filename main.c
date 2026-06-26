#define FB_BASE     0xA8000000
#define FB_WIDTH    540
#define FB_HEIGHT   960

typedef unsigned int uint32_t;

#define FB_BPP      3

static void fill_rect(unsigned char *fb, int x, int y, int w, int h, 
                      unsigned char r, unsigned char g, unsigned char b) {
    for (int row = y; row < y + h; row++) {
        for (int col = x; col < x + w; col++) {
            int offset = (row * FB_WIDTH + col) * FB_BPP;
            fb[offset + 0] = r;
            fb[offset + 1] = g;
            fb[offset + 2] = b;
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

void main(void *dtb, void *kernel) {
	watchdog_disable();

    unsigned char *fb = (unsigned char *)FB_BASE;

    // clear to black
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT * FB_BPP; i++)
        fb[i] = 0xAA;

    // fill_rect(fb,   0,   0, 180, 320, 255,   0,   0); // red
    // fill_rect(fb, 180,   0, 180, 320,   0, 255,   0); // green
    // fill_rect(fb, 360,   0, 180, 320,   0,   0, 255); // blue
    fill_rect(fb,   0, 320, 180, 320, 255, 255,   0); // yellow
    // fill_rect(fb, 180, 320, 180, 320,   0, 255, 255); // cyan
    // fill_rect(fb, 360, 320, 180, 320, 255,   0, 255); // magenta
    // fill_rect(fb,   0, 640, 180, 320, 255, 255, 255); // white
    // fill_rect(fb, 180, 640, 180, 320, 128, 128, 128); // gray
    // fill_rect(fb, 360, 640, 180, 320,  64,  64,  64); // dark gray

    while(1) {
        watchdog_kick(); // if disable doesn't work, kick it in the loop
    }
}