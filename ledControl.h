#include "MKL25Z4.h"          // Device header

#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define SW_POS 6
#define MASK(x) (1 << (x))

void control_RGB_LEDs(int controller) {
	switch (controller){
		case 0:
			PTB->PDOR = 1 << RED_LED;
			PTD->PSOR |= 1 << BLUE_LED;
		case 1:
			PTB->PDOR |= 1 << GREEN_LED;
			PTD->PSOR |= 1 << BLUE_LED;	
		case 2:
			PTD->PDOR |= 1 << BLUE_LED;
			PTB->PSOR |= (MASK(RED_LED) | MASK(GREEN_LED));
	}
}