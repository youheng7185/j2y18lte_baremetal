#include "timer.h"

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

#define WDT_BASE        0x0B017000
#define WDT_RST         (*(volatile unsigned int *)(WDT_BASE + 0x04))
#define WDT_EN          (*(volatile unsigned int *)(WDT_BASE + 0x08))

void watchdog_disable(void) {
    WDT_EN = 0;
}

void watchdog_kick(void) {
    WDT_RST = 1;
}
