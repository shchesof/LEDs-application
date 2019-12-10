#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <pthread.h>

#include "mzapo_parlcd.h"
#include "mzapo_phys.h"
#include "mzapo_regs.h"
#include "lcdframe.h"
#include "led.h"
#include "rgb.h"

_Bool settingsMenu, mainMenu, largerFont, needToStop, phaseChanged;
_Bool isOnTimerLeft, isExtLeft, isIlLeft;
_Bool isOnTimerRight, isExtRight, isIlRight;
_Bool isOnTimerBoth;
_Bool rightShouldEx, rightShouldIll;

double hLeft = 0, sLeft = 0, vLeft = 0;
double hRight = 0, sRight = 0, vRight = 0;
double hBoth = 0, sBoth = 0, vBoth = 0;
double h = 0, s = 0, v = 0;

int illuminationLeft = 60, illuminationRight = 60;
int extinctionLeft = 0, extinctionRight = 0;
int illumination = 0, extinction = 0;

int phaseShift = 0, prevPhaseShift = 0;

int transition_set_left = 0;
int transition_set_right = 0;
int transition_set_both = 0;


int need_to_cancel = 0;

struct timespec tim, tim2;
struct timespec phaseTim, phaseTim2;

struct timeval tval_before;

time_t startIllLeft, startExtLeft;
time_t startIllRight, startExtRight;

pthread_t flashThreadLeft;
pthread_t flashThreadRight;

void chooseFont();
void drawMainMenu();
void drawSettings(char* led);
void drawFlashingSettings();
void illuminationTimeoutLeft();
void extinctionTimeoutLeft();
void illuminationTimeoutRight();
void extinctionTimeoutRight();
void changeLEDSlightning(char* led, double hh, double ss, double vv);
void *flashingLeft(void *ledPtr);
void *flashingRight(void *ledPtr);
void *left_light_transition();
void *right_light_transition();
void *both_lights_transition();
