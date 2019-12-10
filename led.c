/*******************************************************************
  Simple program for MZAPO board that controls two RGB reflectors.

  main_app.c - main file

  (C) Copyright 2019-2019 by Nikita Dvoriadkin and Sofia Shchepetova
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "led.h"
#include "font_types.h"

unsigned char *mem_base = NULL;
int rk, gk, bk, rb, gb, bb;
uint32_t rgb_knobs_value;

/* Peripheral initialization */
void initLEDs() {
	mem_base = map_phys_address(SPILED_REG_BASE_PHYS, SPILED_REG_SIZE, 0);
	*(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0xFFFFFFFF;
	// to light on 2 RGB LEDs
}

void ligth_LEDs(int which, uint32_t color) {
	if (which == 1) *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = color; 
	else if (which == 2) *(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = color;
	else {
		*(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB1_o) = color;
		*(volatile uint32_t*)(mem_base + SPILED_REG_LED_RGB2_o) = color;
	}
	
}

void initKNOBs() {
	rgb_knobs_value = *(volatile uint32_t*)(mem_base + SPILED_REG_KNOBS_8BIT_o);
	bk =  rgb_knobs_value      & 0xFF; // blue knob position
	gk = (rgb_knobs_value>>8)  & 0xFF; // green knob position
	rk = (rgb_knobs_value>>16) & 0xFF; // red knob position
	bb = (rgb_knobs_value>>24) & 1;    // blue button
	gb = (rgb_knobs_value>>25) & 1;    // green button
	rb = (rgb_knobs_value>>26) & 1;    // red buttom
}


/* Convertation functions */
