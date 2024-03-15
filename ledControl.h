#include "MKL25Z4.h"          // Device header
#include "stdbool.h"
#include "constants.h"

void control_RGB_LEDs(int controller, bool state) {
	switch (controller){
		case 0:
			PTB->PDOR = state << RED_LED;
		case 1:
			PTB->PDOR = state << GREEN_LED;
		case 2:
			PTD->PDOR = state << BLUE_LED;
	}
}
