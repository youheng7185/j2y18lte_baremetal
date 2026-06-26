#ifndef TIMER_H
#define TIMER_H

unsigned int hal_gettick_ms(void);
void hal_delay_ms(unsigned int ms);
void watchdog_disable(void);
void watchdog_kick(void);

#endif