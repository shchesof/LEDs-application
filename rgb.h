#ifndef _RGB_H_
#define _RGB_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


uint32_t toRGB(unsigned char r, unsigned char g, unsigned char b);
uint32_t hsvToRGB(double hh, double ss, double vv);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
