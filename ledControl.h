#include "MKL25Z4.h"          // Device header
#include "constants.h"

void control_RGB_LEDs(int controller, bool state) {
	switch (controller){
		case 0:
			PTB->PDOR = state << RED_LED;
			PTB->PCOR = 1 << GREEN_LED;
			PTD->PCOR = 1 << BLUE_LED;
			break;
		case 1:
			PTB->PDOR = state << GREEN_LED;
			PTB->PCOR = 1 << RED_LED;
			PTD->PCOR = 1 << BLUE_LED;
			break;
		case 2:
			PTD->PDOR = state << BLUE_LED;
			PTB->PCOR = 1 << RED_LED;
			PTB->PCOR = 1 << GREEN_LED;
			break;
		default:
			PTB->PCOR = 1 << RED_LED;
			PTB->PCOR = 1 << GREEN_LED;
			PTD->PCOR = 1 << BLUE_LED;
			break;
	}
}
