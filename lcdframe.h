#ifndef _LCDFRAME_H_
#define _LCDFRAME_H_

#include <stdint.h>

#define FRAME_H 320
#define FRAME_W 480

#ifdef __cplusplus
extern "C" {
#endif


extern unsigned char *parlcd_mem_base;

extern uint16_t frame [FRAME_H][FRAME_W]; // velikost displeje

void frameToLCD();
int strToFrame(char *str, int yRow, int xColumn, uint16_t forecolor, uint16_t backcolor,
_Bool largerFont);
int charToFrame(char c, int yRow, int xColumn, uint16_t forecolor, uint16_t backcolor,
_Bool largerFont);



#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
