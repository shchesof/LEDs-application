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

uint32_t toRGB(unsigned char r, unsigned char g, unsigned char b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

uint32_t hsvToRGB(double hh, double ss, double vv) {
	unsigned char R, G, B;
	double r = 0, g = 0, b = 0;

	if (ss == 0)
	{
		r = vv;
		g = vv;
		b = vv;
	}
	else
	{
		int i;
		double f, p, q, t;

		if (hh == 360)
			hh = 0;
		else
			hh = hh / 60;

		i = (int)trunc(hh);
		f = hh - i;

		p = vv * (1.0 - ss);
		q = vv * (1.0 - (ss* f));
		t = vv * (1.0 - (ss * (1.0 - f)));

		switch (i)
		{
		case 0:
			r = vv;
			g = t;
			b = p;
			break;

		case 1:
			r = q;
			g = vv;
			b = p;
			break;

		case 2:
			r = p;
			g = vv;
			b = t;
			break;

		case 3:
			r = p;
			g = q;
			b = vv;
			break;

		case 4:
			r = t;
			g = p;
			b = vv;
			break;

		default:
			r = vv;
			g = p;
			b = q;
			break;
		}

	}

	R = r * 255;
	G = g * 255;
	B = b * 255;
	uint32_t rgbColor = toRGB(R, G, B);
	return rgbColor;
}
