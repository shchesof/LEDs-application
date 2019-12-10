#ifndef _LED_H_
#define _LED_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


extern unsigned char *mem_base;
extern uint32_t rgb_knobs_value;
extern unsigned char R, G, B;;
extern int rk, gk, bk, rb, gb, bb;

void initLEDs();
void initKNOBs();
void ligth_LEDs(int which, uint32_t color);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

