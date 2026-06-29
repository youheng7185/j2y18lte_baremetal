#include <stdint.h>
#include "display.h"
#include "timer.h"
#include "gpio.h"

#define VOL_KEY_GPIO   127   // bias-pull-up → LOW when pressed
#define HOME_KEY_GPIO  128   // no pull → check hardware
#define TORCH_GPIO     34    // cam_flash TORCH

extern char dtb[];
extern char kernel[];
extern char initramfs[];
extern char initramfs_end[];

#define INITRD_START 0x82000000
#define INITRD_END   0x823d87bb

void boot_kernel(void *dtb, int a1, int a2, int a3, void *entry);

static void memcpy_(void *dst, const void *src, unsigned long n)
{
    unsigned char *d = dst;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
}

void main(void *dtb, void *kernel)
{
    unsigned long rd_size = (unsigned long)(initramfs_end - initramfs);

    /* copy initramfs to where bootloader would normally place it */
    memcpy_((void *)0x82000000, initramfs, rd_size);

    /* copy kernel image to its expected load address */
    memcpy_((void *)0x90000000, kernel, 0x2000000);

    /* hand off: x0=dtb, x1=0, x2=0, x3=0, x4=entry */
    boot_kernel(dtb, 0, 0, 0, (void *)0x90000000);
}

// void main(void *dtb, void *kernel) {
// 	watchdog_disable();

//     unsigned char *fb = (unsigned char *)FB_BASE;

//     // clear to white
//     for (int i = 0; i < FB_WIDTH * FB_HEIGHT * FB_BPP; i++)
//         fb[i] = 0x00;

//     draw_string(fb, 10, 10, "GPIO button test", 255, 255, 255, 0, 0, 0);

//     // Configure buttons as inputs
//     // Vol key: pull-up, 2mA (matches DTS: bias-pull-up, drive-strength=2)
//     gpio_config_input(VOL_KEY_GPIO,  GPIO_PULL_UP, GPIO_DRV_2MA);
//     // Home key: no pull, 2mA (matches DTS: bias-disable, drive-strength=2)
//     gpio_config_input(HOME_KEY_GPIO, GPIO_PULL_NONE, GPIO_DRV_2MA);
//     gpio_config_output(TORCH_GPIO, GPIO_PULL_DOWN, GPIO_DRV_2MA);

//     int torch_state = 0;
//     int last_vol  = 1;

//     while (1) {
//         int vol  = gpio_read(VOL_KEY_GPIO);   // 0 = pressed (pull-up, active low)
//         int home = gpio_read(HOME_KEY_GPIO);  // depends on your hardware wiring

//         if (last_vol == 1 && vol == 0) {
//             torch_state ^= 1;
//             gpio_write(TORCH_GPIO, torch_state);

//             if (torch_state)
//                 draw_string(fb, 10, 40, "volume pressed TORCH ON ", 255, 255, 0, 0, 0, 0);
//             else
//                 draw_string(fb, 10, 40, "volume released TORCH OFF", 255, 255, 255, 0, 0, 0);
//         }

//         if (!home)
//             draw_string(fb, 10, 80, "HOME pressed!", 0, 255, 0, 0, 0, 0);
//         else
//             draw_string(fb, 10, 80, "HOME released", 255, 255, 255, 0, 0, 0);

//         last_vol  = vol;

//         hal_delay_ms(50);  // debounce
//     }

// 	draw_string(fb, 10, 10, "Hello World from j2y18lte", 255, 255, 255, 0, 0, 0);

// 	// fill_rect(fb,   0,   0, 180, 320, 255,   0,   0); // red
//     // fill_rect(fb, 180,   0, 180, 320,   0, 255,   0); // green
//     // fill_rect(fb, 360,   0, 180, 320,   0,   0, 255); // blue
//     // fill_rect(fb,   0, 320, 180, 320, 255, 255,   0); // yellow
//     // fill_rect(fb, 180, 320, 180, 320,   0, 255, 255); // cyan
//     // fill_rect(fb, 360, 320, 180, 320, 255,   0, 255); // magenta
//     // fill_rect(fb,   0, 640, 180, 320, 255, 255, 255); // white
//     // fill_rect(fb, 180, 640, 180, 320, 128, 128, 128); // gray
//     // fill_rect(fb, 360, 640, 180, 320,  64,  64,  64); // dark gray

//     hal_delay_ms(1000);

//     // clear once
//     for (int i = 0; i < FB_WIDTH * FB_HEIGHT * FB_BPP; i++)
//         fb[i] = 0x00;

//     uint8_t count = 0;

//     while(1) {
//         if (count % 2 == 0) {
//             draw_string(fb, 10, 50, "tick even!", 255, 255, 255, 0, 0, 0);
//         } else {
//             draw_string(fb, 10, 50, "tick odd!", 255, 255, 255, 0, 0, 0);
//         }
//         count++;
//         hal_delay_ms(1000);
//     }
// }