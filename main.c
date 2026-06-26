#include <stdint.h>
#include "display.h"
#include "timer.h"

void main(void *dtb, void *kernel) {
	watchdog_disable();

    unsigned char *fb = (unsigned char *)FB_BASE;

    // clear to white
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT * FB_BPP; i++)
        fb[i] = 0x00;

	draw_string(fb, 10, 10, "Hello World from j2y18lte", 255, 255, 255, 0, 0, 0);

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