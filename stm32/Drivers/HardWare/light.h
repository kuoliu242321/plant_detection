#ifndef __LIGHT_H
#define __LIGHT_H
#include "./SYSTEM/sys/sys.h"
void led_init(void);
void led_off(void);
void led_on(void);
//void led_conctrl(uint8_t *buffer, size_t size);
void led_conctrl(char *buffer1,char *buffer2);
#endif
