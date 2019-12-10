/*******************************************************************
  Simple program for MZAPO board that controls two RGB reflectors.

  main_app.c - main file

  (C) Copyright 2019-2019 by Nikita Dvoriadkin and Sofia Shchepetova
      license:  any combination of GPL, LGPL, MPL or BSD licenses

 *******************************************************************/
 
#define _DEFAULT_SOURCE
#define _POSIX_C_SOURCE 200112L

#include "main_app.h"

int main(int argc, char *argv[])
{
	printf("Hello MZAPO\n");
	
    tim.tv_sec = 0;
    tim.tv_nsec = 300000000L;
	
	/* MicroZed peripheral initialization */
	parlcd_mem_base = map_phys_address(PARLCD_REG_BASE_PHYS, PARLCD_REG_SIZE, 0);
	
	if (parlcd_mem_base == NULL) {
		printf("Err: map_phys_address\n");
		exit(1);
	}
	
	parlcd_hx8357_init(parlcd_mem_base); 
	initLEDs();
	
	
	/* View Controllers */
	chooseFont();
	drawMainMenu();
	
	return 0;
}

void chooseFont() {
	while (1) {
		
		initKNOBs();
		
		if ((rk >= 120-5) && (rk <= 120+5) && (rb==1)) {
			largerFont = 1;
			drawMainMenu();
			break;
		}
		if ((rk >= 170-5) && (rk <= 170+5) && (rb==1)) {
			largerFont = 0;
			drawMainMenu();
			break;
		}
		
		for (int i = 0; i < FRAME_H; i++) {
			for (int j = 0; j < FRAME_W; j++) {
				frame[i][j] = 0x0; // black
			}
		}
		
		strToFrame("CHOOSE SIZE OF FONT", 70, 130, 0x1F, 0xFFE0, 0);
		
		if ((rk >= 120-5) && (rk <= 120+5))
			strToFrame("LARGER", 120, 190, 0x1F, 0x780F, 1);
		else 
			strToFrame("LARGER", 120, 190, 0x1F, 0xFFE0, 1);
			
		if ((rk >= 170-5) && (rk <= 170+5))
			strToFrame("smaller", 170, 200, 0x1F, 0x780F, 0);
		else strToFrame("smaller", 170, 200, 0x1F, 0xFFE0, 0);
		
		charToFrame('X', rk, 50, 0x1F, 0xFFE0, largerFont);
		frameToLCD();
	}
}

void drawMainMenu() {
	rk = 0, rb = 0, gb = 0;
	int count = 0;
	int i, j;
	while (1) {
		
		pthread_t thread1;
		pthread_t thread2;
		pthread_t thread3;
		
		if ((settingsMenu) && (gb==1)) {
			settingsMenu = 0;
			continue;
		}
		if (count == 0) nanosleep(&tim , &tim2);
		initKNOBs();
		
		if ((rk >= 100-5) && (rk <= 100+5) && (rb==1)) {
			char *side = "LEFT";
			drawSettings(side);
		}
		
		if ((rk >= 150-5) && (rk <= 150+5) && (rb==1)) {
			char *side = "RIGHT";
			drawSettings(side);
		}
		
		if ((rk >= 200-5) && (rk <= 200+5) && (rb==1)) {
			char *side = "BOTH";
			drawSettings(side);
		}
		
		/* Set transition mode for LEFT LED */
        if (gb == 1 && rb == 1) {
            nanosleep(&tim , &tim2);
            
            if (transition_set_left == 0) {
                transition_set_left = 1;
                nanosleep(&tim , &tim2);
                pthread_create(&thread1, NULL, left_light_transition, NULL);
            } else {
                transition_set_left = 0;
                pthread_cancel(thread1);
            }
        }
        
        /* Set transition mode for RIGHT LED */
        if (bb == 1 && gb == 1) {
            nanosleep(&tim , &tim2);
            
            if (transition_set_right == 0) {
                transition_set_right = 1;
                nanosleep(&tim , &tim2);
                pthread_create(&thread2, NULL, right_light_transition, NULL);
            } else if (transition_set_right == 1){
                transition_set_right = 0;
                pthread_cancel(thread2);
            }
        }
        
        /* Set transition mode for BOTH LEDs */
        if (bb == 1 && rb == 1) {
            nanosleep(&tim , &tim2);
            
            if (transition_set_left == 1)  {      // if light transition was set
                pthread_cancel(thread1);          // for LEFT LED, it would be
                transition_set_left = 0;          // turned off
                printf("Left thread was turned off\n");
            }
            
            if (transition_set_right == 1) {      // if light transition was set
                pthread_cancel(thread2);           // for RIGHT LED, it would be
                transition_set_right = 0;         // turned off
                printf("right thread was turned off\n");
            }
            
            
            pthread_create(&thread3, NULL, both_lights_transition, NULL);
        }
		
		// Exit button pressed
		if ((rk >= 250-5) && (rk <= 250+5) && (rb==1)) {
			for (i = 0; i < FRAME_H; i++) {
				for (j = 0; j < FRAME_W; j++) {
					frame[i][j] = 0x0; 
				}
			}
			needToStop = 1;
			strToFrame("Bye!", 150, 200, 0x1F, 0xFFE0, largerFont);
			frameToLCD();
			ligth_LEDs(3, 0);
			nanosleep(&tim , &tim2);
			*(volatile uint32_t*)(mem_base + SPILED_REG_LED_LINE_o) = 0x0;
			pthread_cancel(flashThreadLeft);
			printf("Goodbye MZAPO\n");
			exit(0); // Exit application  
		}

		for (i = 0; i < FRAME_H; i++) {
			for (j = 0; j < FRAME_W; j++) {
				frame[i][j] = 0x0; // black
			}
		}
		strToFrame("Main menu", 50, 180, 0x1F, 0xFFE0, largerFont);
		
		if ((rk >= 100-5) && (rk <= 100+5))
			strToFrame("Left LED settings", 100, 140, 0x1F, 0x780F, largerFont);
		else 
			strToFrame("Left LED settings", 100, 140, 0x1F, 0xFFE0, largerFont);
			
		if ((rk >= 150-5) && (rk <= 150+5))
			 strToFrame("Right LED settings", 150, 140, 0x1F, 0x780F, largerFont);
		else
			strToFrame("Right LED settings", 150, 140, 0x1F, 0xFFE0, largerFont);
		
		if ((rk >= 200-5) && (rk <= 200+5))
			strToFrame("Both LED Settings", 200, 140, 0x1F, 0x780F, largerFont);
		else
			strToFrame("Both LED settings", 200, 140, 0x1F, 0xFFE0, largerFont);
		
		if ((rk >= 250-5) && (rk <= 250+5))
			strToFrame("Exit", 250, 210, 0x1F, 0x780F, largerFont);
		else
			strToFrame("Exit", 250, 210, 0x1F, 0xFFE0, largerFont);
		
		charToFrame('X', rk, 50, 0x1F, 0xFFE0, largerFont);
		
		frameToLCD();
		count++;
	}	
}

void drawSettings(char* led) {
	settingsMenu = 1;
	int trk = 0, tgk = 0, tbk = 0, count = 0;
	char hsvString[30];
	while (1) {
		for (int i = 0; i < FRAME_H; i++) {
			for (int j = 0; j < FRAME_W; j++) {
				frame[i][j] = 0x0; 
			}
		}
		
		if (count==0)
			nanosleep(&tim , &tim2);
		initKNOBs();
		
		if (strstr(led, "LEFT")) {
			strToFrame("LEFT LED WAS CHOSEN", 100, 100, 0x1F, 0xFFE0, largerFont);
			strToFrame("Press blue b. to copy from RIGHT", 280, 0, 0x1F, 0xFFE0, largerFont);
			h = hLeft, s = sLeft, v = vLeft;
		}
		else if (strstr(led, "RIGHT")) {
			strToFrame("RIGHT LED WAS CHOSEN", 100, 100, 0x1F, 0xFFE0, largerFont);
			strToFrame("Press b. b. to copy from LEFT", 280, 10, 0x1F, 0xFFE0, largerFont);
			h = hRight, s = sRight, v = vRight;
		}
		else {
			strToFrame("BOTH LEDS WERE CHOSEN", 100, 100, 0x1F, 0xFFE0, largerFont);
			h = hBoth, s = sBoth, v = vBoth;
		}
		
		if (count > 0) {
			if (rk > trk) {
				h ++;;
				if (h>360) h = 0;
			}
			else if (rk < trk) {
				h --;;
				if (h<0) h = 360;
			}
			if (gk > tgk) {
				s += 0.1;
				if (s>100) s = 0;
			}
			else if (gk < tgk) {
				s -= 0.1;
				if (s<0) s = 100;
			}
			if (bk > tbk) {
				v += 0.1;
				if (v>100) v = 0;
			}
			else if (bk < tbk) {
				v -= 0.1;
				if (v<0) v = 100;
			}
		}
		
		snprintf(hsvString, 30, "H: %.2f S: %.2f V: %.2f", h, s, v);
		strToFrame(hsvString, 220, 50, 0x1F, 0xFFE0, largerFont);
		
		if ((strstr(led, "LEFT")) && bb == 1) {
			h = hRight, s = sRight, v = vRight;
		}
		
		if (strstr(led, "RIGHT") && bb == 1) {
			h = hLeft, s = sLeft, v = vLeft;
		}

		strToFrame("Press r.b for flash set.", 170, 50, 0x1F, 0xFFE0, largerFont);
		if (rb == 1) {
			drawFlashingSettings(led);
		}

		uint32_t rgbColor = hsvToRGB(h, s, v);
		if (strstr(led, "LEFT")) {
			if (!isOnTimerLeft) ligth_LEDs(1, rgbColor);
			hLeft = h, sLeft = s, vLeft = v;
		}
		else if (strstr(led, "RIGHT")) {
			if (!isOnTimerRight) ligth_LEDs(2, rgbColor);
			hRight = h, sRight = s, vRight = v;
		}
		else {
			if (!(isOnTimerLeft || isOnTimerRight))ligth_LEDs(3, rgbColor);
			hBoth = h, sBoth = s, vBoth = v;
			hLeft = h, sLeft = s, vLeft = v;
			hRight = h, sRight = s, vRight = v;
		}

		frameToLCD();
		
		trk = rk, tgk = gk, tbk = bk;
		count++;
		if (gb==1) break;
	}
	drawMainMenu();
}

void drawFlashingSettings(char *led) {
	char illuminationStr[22];
	int trk = 0, tgk = 0, tbk = 0, count = 0;
	while (1) {
		if (count==0)
			nanosleep(&tim , &tim2);
			
		initKNOBs();
		
		for (int i = 0; i < FRAME_H; i++) {
			for (int j = 0; j < FRAME_W; j++) {
				frame[i][j] = 0x0; 
			}
		}
		
		if (strstr(led, "LEFT")) {
			strToFrame("Time intervals for LEFT", 100, 100, 0x1F, 0xFFE0, largerFont);
			illumination = illuminationLeft;
			extinction = extinctionLeft;
		}
		
		
		if (strstr(led, "RIGHT")) {
			strToFrame("Time intervals for RIGHT", 100, 100, 0x1F, 0xFFE0, largerFont);
			illumination = illuminationRight;
			extinction = extinctionRight;
		}
		
		if (strstr(led, "BOTH")) {
			strToFrame("Time intervals for BOTH", 100, 100, 0x1F, 0xFFE0, largerFont);
			illumination = illuminationRight;
			extinction = extinctionRight;
		}
		
		if (count > 0) {
			if (rk > trk) {
				illumination ++;
				if (illumination>60) illumination = 0;
			}
			else if (rk < trk) {
				illumination --;
				if (illumination<0) illumination = 60;
			}
			if (gk > tgk) {
				extinction ++;
				if (extinction>60) extinction = 0;
			}
			else if (gk < tgk) {
				extinction --;
				if (extinction<0) extinction = 60;
			}
			// it's possible to choose phase only if LEDs have the same
			// illumination and extinction values
			if ((illuminationLeft == illuminationRight) &&
				(extinctionLeft == extinctionRight)) {
				if (bk > tbk) {
					phaseShift += 100;
					if (phaseShift>(illumination+extinction)*1000) phaseShift = 0;
				}
				else if (bk < tbk) {
					phaseShift -= 100;
					if (phaseShift<0) phaseShift = 1000;
				}
			}
			else phaseShift = 0;
		}
		
		if (prevPhaseShift != phaseShift) phaseChanged = 1;
		
		snprintf(illuminationStr, 22, "IL: %d EX: %d PH: %d", illumination,
		extinction, phaseShift);
		strToFrame(illuminationStr, 170, 100, 0x1F, 0xFFE0, largerFont);
		strToFrame("Press b.b. to start timer", 210, 30, 0x1F, 0xFFE0, largerFont);
			
		if ((bb == 1) && (!isOnTimerLeft) && (strstr(led, "LEFT"))) {
			strToFrame("Press b.b. to start timer", 210, 30, 0x1F, 0x780F, largerFont);
			time(&startIllLeft);
			gettimeofday(&tval_before, NULL);
			isOnTimerLeft = 1;
			isIlLeft = 1, isExtLeft = 0;
			pthread_create(&flashThreadLeft, NULL, flashingLeft, &led);
		}
		
		if ((bb == 1) && (!isOnTimerRight) && (strstr(led, "RIGHT"))) {
			strToFrame("Press b.b. to start timer", 210, 30, 0x1F, 0x780F, largerFont);
			time(&startIllRight);
			isOnTimerRight = 1;
			isIlRight = 1, isExtRight = 0;
			pthread_create(&flashThreadRight, NULL, flashingRight, &led);
		}
		
		if ((bb == 1) && (!isOnTimerBoth) && (strstr(led, "BOTH"))) {
			strToFrame("Press b.b. to start timer", 210, 30, 0x1F, 0x780F, largerFont);
			time(&startIllRight);
			time(&startIllLeft);
			isOnTimerBoth = 1;
			isOnTimerLeft = 1;
			isOnTimerRight = 1;
			isIlRight = 1, isExtRight = 0;
			isIlLeft = 1, isExtLeft = 0;
			pthread_create(&flashThreadRight, NULL, flashingRight, &led);
			pthread_create(&flashThreadLeft, NULL, flashingLeft, &led);
		}
		
		if (strstr(led, "LEFT")) {
			illuminationLeft = illumination, extinctionLeft = extinction;
		}
		else if (strstr(led, "RIGHT")) {
			illuminationRight = illumination, extinctionRight = extinction;
		}
		else {
			illuminationLeft = illumination, extinctionLeft = extinction;
			illuminationRight = illumination, extinctionRight = extinction;
		}
	
		frameToLCD();
		trk = rk, tgk = gk, tbk = bk;
		prevPhaseShift = phaseShift;
		count++;
		if (gb == 1) break;
	}
}

void illuminationTimeoutLeft() {
	double diffIllumination = 0.0;
	time_t stop;
	time(&stop);
	diffIllumination = difftime(stop, startIllLeft);
	if (diffIllumination == illuminationLeft) {
		time(&startExtLeft);
		changeLEDSlightning("LEFT", 0, 0, 0);
		isExtLeft = 1;
		isIlLeft = 0;
	}
}

void illuminationTimeoutRight() {
	double diffIllumination = 0.0;
	time_t stop;
	time(&stop);
	diffIllumination = difftime(stop, startIllRight);
	if ((diffIllumination == illuminationRight) || (rightShouldEx)) {
		printf("Ext right LED\n");
		time(&startExtRight);
		changeLEDSlightning("RIGHT", 0, 0, 0);
		isExtRight = 1;
		isIlRight = 0;
		rightShouldEx = 0;
	}
}

void extinctionTimeoutLeft() {
	double diffExtinction = 0.0;
	time_t stop;
	time(&stop);
	diffExtinction = difftime(stop, startExtLeft);
	if (diffExtinction == extinctionLeft) {
		time(&startIllLeft);
		gettimeofday(&tval_before, NULL);
		changeLEDSlightning("LEFT", hLeft, sLeft, vLeft);
		isIlLeft = 1;
		isExtLeft = 0;
	}
}

void extinctionTimeoutRight() {
	double diffExtinction = 0.0;
	time_t stop;
	time(&stop);
	diffExtinction = difftime(stop, startExtRight);
	if ((rightShouldIll) || (diffExtinction == extinctionRight)) {
		printf("Light up right LED\n");
		time(&startIllRight);
		changeLEDSlightning("RIGHT", hRight, sRight, vRight);
		isIlRight = 1;
		isExtRight = 0;
		rightShouldIll = 0;
	}
}

void *flashingLeft(void *ledPtr) {
	printf("Getting into flashingLeft\n");
	//nanosleep(&tim , &tim2);
	//char *led = (char*)ledPtr;
	while(1) {
		if (needToStop) {
			ligth_LEDs(3, 0);
			break;
		}
		if (isIlLeft) 
			illuminationTimeoutLeft();
		else if (isExtLeft)
			extinctionTimeoutLeft();
	}
}

void *flashingRight(void *ledPtr) {
	printf("Getting into flashingRight\n");
	//nanosleep(&tim , &tim2);
	//char *led = (char*)ledPtr;
	while(1) {
		if (needToStop) {
			ligth_LEDs(3, 0);
			break;
		}
		if (phaseChanged) {
			struct timeval tval_after, tval_result;
			gettimeofday(&tval_after, NULL);
			timersub(&tval_after, &tval_before, &tval_result);
			printf("Time elapsed: %ld.%06ld\n", (long int)tval_result.tv_sec,
				(long int)tval_result.tv_usec);
			struct timespec demoRight;
			demoRight.tv_sec = tval_result.tv_sec;
			if (tval_result.tv_usec + phaseShift*1000 > 1000000) {
				demoRight.tv_sec ++;
				demoRight.tv_nsec = (tval_result.tv_usec +
					phaseShift*1000 - 1000000)*1000;
			}
			else demoRight.tv_nsec = (tval_result.tv_usec + phaseShift*1000)*1000;
			printf("New time: %ld.%06ld\n", (long int)demoRight.tv_sec,
				(long int)demoRight.tv_nsec);
			printf("Phase shift: %d\n", phaseShift);
			if (demoRight.tv_sec > illumination + extinction) {
				printf("HERE1\n");
				demoRight.tv_sec -= illumination + extinction;
				demoRight.tv_sec = illumination - demoRight.tv_sec;
				demoRight.tv_nsec = 1000000000 - demoRight.tv_nsec;
				printf("Time to do action: %ld.%06ld\n", (long int)demoRight.tv_sec,
					(long int)demoRight.tv_nsec);
				if (demoRight.tv_sec + demoRight.tv_nsec < illumination) {
					changeLEDSlightning("RIGHT", hRight, sRight, vRight);
					nanosleep(&demoRight, &phaseTim2);
					printf("Wake up1\n");
					rightShouldEx = 1;
					isIlRight = 1;
					isExtRight = 0;
				}
				else {
					changeLEDSlightning("RIGHT", 0, 0, 0);
					nanosleep(&demoRight, &phaseTim2);
					printf("Wake up1\n");
					rightShouldIll = 1;
					isExtRight = 1;
					isIlRight = 0;
				}
			}
			else if (demoRight.tv_sec < illumination) {
				printf("HERE2\n");
				demoRight.tv_nsec = 1000000000 - demoRight.tv_nsec;
				if (demoRight.tv_sec > 1)
					demoRight.tv_sec = illumination - demoRight.tv_sec;
				//changeLEDSlightning("RIGHT", hRight, sRight, vRight);
				printf("Time to do action: %ld.%06ld\n", (long int)demoRight.tv_sec,
					(long int)demoRight.tv_nsec);
				nanosleep(&demoRight, &phaseTim2);
				printf("Wake up2\n");
				rightShouldEx = 1;
				isIlRight = 1;
				isExtRight = 0;
			}
			else if ((demoRight.tv_sec == illumination) && (demoRight.tv_nsec == 0)) {
				printf("HERE3\n");
				rightShouldEx = 1;
				isIlRight = 1;
				isExtRight = 0;
			}
			else {
				printf("HERE4\n");
				isExtRight = 1;
				isIlRight = 0;
				rightShouldIll = 1;
				changeLEDSlightning("RIGHT", 0, 0, 0);
				double secs = extinction + illumination -
					demoRight.tv_sec - demoRight.tv_nsec/1000000000.0;
				demoRight.tv_sec = (int)secs;
				demoRight.tv_nsec = 1000000000 - demoRight.tv_nsec;
				printf("Time to do action: %ld.%06ld\n", (long int)demoRight.tv_sec,
					(long int)demoRight.tv_nsec);
				nanosleep(&demoRight, &phaseTim2);
				printf("Wake up4\n");
			}
			phaseChanged = 0;
		}
		if (isIlRight) 
			illuminationTimeoutRight();
		else if (isExtRight)
			extinctionTimeoutRight();
	}
}

void changeLEDSlightning(char* led, double hh, double ss, double vv) {
	int ledNumber;
	if (strstr(led, "LEFT")) ledNumber = 1;
	else ledNumber = 2;
	uint32_t rgbColor = hsvToRGB(hh, ss, vv);
	ligth_LEDs(ledNumber, rgbColor);
}


/* Functions to transit LED lights, passed into thread in drawMainMenu() func */
void *left_light_transition() {
    int j = 0;
    
    while(1) {
		
		if (needToStop) {
			ligth_LEDs(3, 0);
			break;
		}
		
        uint32_t rgb_color = hsvToRGB(hLeft, sLeft, vLeft);
        ligth_LEDs(1, rgb_color);
        
        if (j < 200) {
            hLeft+=0.01, sLeft+=0.002, vLeft+=0.005 ;
            hLeft = fmod(hLeft, 360);
            sLeft = fmod(sLeft, 50);
            vLeft = fmod(vLeft, 50);
        } else if (j > 198 && j < 400) {
            hLeft-=0.01, sLeft-=0.002, vLeft-=0.005 ;
            hLeft = fmod(hLeft, 360);
            sLeft = fmod(sLeft, 50);
            vLeft = fmod(vLeft, 50);
        } else j = 0;
        j += 1;
        
        usleep(10000);
        
        if (need_to_cancel) {
            ligth_LEDs(1, 0);
            break;
        }
    }
    
}
void *right_light_transition() {
    int j = 0;
    printf("Got into the thread\n");
    printf("%d\n", transition_set_right);
    while(1) {
		
		if (needToStop) {
			ligth_LEDs(3, 0);
			break;
		}
		
        uint32_t rgb_color = hsvToRGB(hRight, sRight, vRight);
        ligth_LEDs(2, rgb_color);
        if (j < 200) {
            hRight+=0.01, sRight+=0.002, vRight+=0.005 ;
            hRight = fmod(hRight, 360);
            sRight = fmod(sRight, 50);
            vRight = fmod(vRight, 50);
        } else if (j > 198 && j < 400) {
            hRight-=0.01, sRight-=0.002, vRight-=0.005 ;
            hRight = fmod(hRight, 360);
            sRight = fmod(sRight, 50);
            vRight = fmod(vRight, 50);
        } else j = 0;
        j += 1;
        
        usleep(10000);
        
        if (need_to_cancel) {
            ligth_LEDs(2, 0);
            break;
        }
    }
}
void *both_lights_transition() {
    int j = 0;
    while(1) {
		
		if (needToStop) {
			ligth_LEDs(3, 0);
			break;
		}
		
        uint32_t rgb_color = hsvToRGB(hBoth, sBoth, vBoth);
        ligth_LEDs(3, rgb_color);
        if (j < 200) {
            hBoth+=0.01, sBoth+=0.002, vBoth+=0.005;
            hBoth = fmod(hBoth, 360);
            sBoth = fmod(sBoth, 50);
            vBoth = fmod(vBoth, 50);
        } else if (j > 198 && j < 400) {
            hBoth-=0.01, sBoth-=0.002, vBoth-=0.005 ;
            hBoth = fmod(hBoth, 360);
            sBoth = fmod(sBoth, 50);
            vBoth = fmod(vBoth, 50);
        } else j = 0;
        j += 1;
        
        usleep(10000);
        if (need_to_cancel) {
            ligth_LEDs(3, 0);
            break;
        }
    }
}
