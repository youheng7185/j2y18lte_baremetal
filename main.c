#include <stdint.h>
#include "display.h"
#include "timer.h"
#include "gpio.h"

#define VOL_KEY_GPIO   127   // bias-pull-up → LOW when pressed
#define HOME_KEY_GPIO  128   // no pull → check hardware

void main(void *dtb, void *kernel) {
	watchdog_disable();

    unsigned char *fb = (unsigned char *)FB_BASE;

    // clear to white
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT * FB_BPP; i++)
        fb[i] = 0x00;

    draw_string(fb, 10, 10, "GPIO button test", 255, 255, 255, 0, 0, 0);

    // Configure buttons as inputs
    // Vol key: pull-up, 2mA (matches DTS: bias-pull-up, drive-strength=2)
    gpio_config_input(VOL_KEY_GPIO,  GPIO_PULL_UP, GPIO_DRV_2MA);
    // Home key: no pull, 2mA (matches DTS: bias-disable, drive-strength=2)
    gpio_config_input(HOME_KEY_GPIO, GPIO_PULL_NONE, GPIO_DRV_2MA);

    while (1) {
        int vol  = gpio_read(VOL_KEY_GPIO);   // 0 = pressed (pull-up, active low)
        int home = gpio_read(HOME_KEY_GPIO);  // depends on your hardware wiring

        if (!vol)
            draw_string(fb, 10, 40, "VOL pressed! ", 255, 255, 0, 0, 0, 0);
        else
            draw_string(fb, 10, 40, "VOL released ", 255, 255, 255, 0, 0, 0);

        if (!home)
            draw_string(fb, 10, 80, "HOME pressed!", 0, 255, 0, 0, 0, 0);
        else
            draw_string(fb, 10, 80, "HOME released", 255, 255, 255, 0, 0, 0);

        hal_delay_ms(50);  // debounce
    }

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