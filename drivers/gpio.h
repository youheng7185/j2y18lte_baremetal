#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

// MSM8916/8917 TLMM base
#define TLMM_BASE       0x01000000UL
#define GPIO_CFG(n)     (*(volatile uint32_t *)(TLMM_BASE + 0x1000UL * (n) + 0x00))
#define GPIO_IN_OUT(n)  (*(volatile uint32_t *)(TLMM_BASE + 0x1000UL * (n) + 0x04))

// TLMM_GPIO_CFG bit fields
#define GPIO_PULL_NONE  (0 << 0)
#define GPIO_PULL_DOWN  (1 << 0)
#define GPIO_PULL_KEEP  (2 << 0)
#define GPIO_PULL_UP    (3 << 0)

#define GPIO_FUNC_GPIO  (0 << 2)   // func_sel = 0 for plain GPIO

// drive strength in bits [8:6]
#define GPIO_DRV_2MA    (0 << 6)
#define GPIO_DRV_4MA    (1 << 6)
#define GPIO_DRV_6MA    (2 << 6)
#define GPIO_DRV_8MA    (3 << 6)

#define GPIO_OE_INPUT   (0 << 9)   // bit9 = 0 → input
#define GPIO_OE_OUTPUT  (1 << 9)   // bit9 = 1 → output

// GPIO_IN_OUT bits
#define GPIO_IN_BIT     (1 << 0)   // read pin state
#define GPIO_OUT_BIT    (1 << 1)   // write pin state

static inline void gpio_config_input(uint32_t pin, uint32_t pull, uint32_t drv) {
    GPIO_CFG(pin) = GPIO_FUNC_GPIO | pull | drv | GPIO_OE_INPUT;
}

static inline int gpio_read(uint32_t pin) {
    return (GPIO_IN_OUT(pin) & GPIO_IN_BIT) ? 1 : 0;
}

#endif