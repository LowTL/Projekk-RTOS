/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#include "constants.h"
#include "progInit.h"
#include "ledControl.h"
#include "buzzerControl.h"
#include "motorControls.h"

volatile uint8_t msg = 0;
volatile uint8_t input = 0; //check if interrupt triggers
volatile uint8_t input2 = 0; //check if motor update triggers
volatile uint8_t right_motor = 0;
volatile uint8_t left_motor = 0;
volatile bool isStationary = false;

uint8_t

osSemaphoreId_t uartSem;
osSemaphoreId_t motorSem;
osSemaphoreId_t ledSem;


typedef struct {
	uint8_t cmd;
	uint8_t data;
} msgPkt;

/*uint8_t UART2_Receive_Poll(void)
{
	//wait until receive data register is full
	while (!(UART2->S1 & UART_S1_RDRF_MASK));
	return (UART2->D);
}
*/

void rightMotor (int input) {
	float right_speed;
	switch(input & (0b11)) { //read last 2 bits to obtain speed
		case 0b00:
			right_speed = 0;
			break;
		case 0b01:
			right_speed = 0.2;
			break;
		case 0b10:
			right_speed = 0.5;
			break;
		case 0b11:
			right_speed = 0.8;
			break;
		default:
			right_speed = 0;
		break;
	}
	if(input & (0b100) { //move fwd
		TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK));
		TPM0_C0SC |= (TPM_CnSC_ELSA(0)) | (TPM_CnSC_ELSB(1));

		TPM0_C0V = TPM0->MOD * (1- right_speed);
		TPM0_C1V = TPM0->MOD -1;

	} else { //move bwd
		TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK));
		TPM0_C0SC |= (TPM_CnSC_ELSA(1)) | (TPM_CnSC_ELSB(1)); //reverse pwm

		TPM0_C0V = TPM0->MOD * right_speed;
		TPM0_C1V = 1;
	}
}

void leftMotor (int input) {
	float left_speed;
	switch(input & (0b11)) { //read last 2 bits to obtain speed
		case 0b00:
			left_speed = 0;
			break;
		case 0b01:
			left_speed = 0.2;
			break;
		case 0b10:
			left_speed = 0.5;
			break;
		case 0b11:
			left_speed = 0.8;
			break;
		default:
			left_speed = 0;
		break;
	}

	if(input & (0b100)) { //move fwd
		TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK));
		TPM0_C3SC |= (TPM_CnSC_ELSA(0)) | (TPM_CnSC_ELSB(1));

		TPM0_C3V = TPM0->MOD * (1-left_speed);
		TPM0_C2V = TPM0->MOD -1;

	} else { //move bwd
		TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK));
		TPM0_C3SC |= (TPM_CnSC_ELSA(1)) | (TPM_CnSC_ELSB(1)); //reverse pwm

		TPM0_C3V = TPM0->MOD * left_speed;
		TPM0_C2V = 1;
	}
}

void UART2_IRQHandler(void) {
	input = input + 1;
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if ((UART2->S1 & UART_S1_RDRF_MASK)) {
		msg = UART2->D;
		osSemaphoreRelease(uartSem);
	}
}

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/

void tBrain (void *argument) {
	for (;;) {
	    osSemaphoreAcquire(uartSem,osWaitForever);
	    right_motor = (msg & 0b00000111);
        left_motor = (msg & 0b00111000) >> 3;

        uint8_t right_motor_speed = (right_motor & 0b11);
        uint8_t left_motor_speed = (left_motor & 0b11);
        isStationary = right_motor_speed == 0 && left_motor_speed == 0;
        osSemaphoreRelease(motorSem, osWaitForever);
	}
	}
}

void tMotorControl (void *argument) {

	for(;;) {
		osSemaphoreAcquire(motorSem,osWaitForever);
		input2 = input2 + 1;

		rightMotor(right_motor);
		leftMotor(left_motor);
	}

}

void tLED (void *argument) {


	for(;;) {
//		osSemaphoreRelease(uartSem);
	}
}

void tAudio (void *argument) {

}

void tLed_red (void *argument) {
    uint32_t delay = 250;

    for(;;) {
    delay = (isStationary) ? 250 : 500;

    PTB->PTOR = 0b01 << RED_LED;
    osDelay(delay);
    }
}

void tLed_green (void *argument) {
    uint8_t i = 0;
    uint8_t j = i + 1;

    for(;;) {
        if (isStationary) {
            PTB->PSOR = 0b11111111 << GREEN_LED; // turn all green LEDs on
        } else {
            i = (i == 7) ? 0 : i + 1;
            j = (i == 7) ? 0 : i + 1;
            PTB->PCOR = 0b01 << (GREEN_LED + i); // turn green LED at position i off
            PTB->PSOR = 0b01 << (GREEN_LED + j); // turn green led at position j on
            osDelay(100);
        }
    }
}

int main (void) {

  // System Initialization
    SystemCoreClockUpdate();
	initUART2();
	initLED();
	initMotors();
  // ...

    osKernelInitialize();                 // Initialize CMSIS-RTOS

	uartSem = osSemaphoreNew(1,0,NULL);
	motorSem = osSemaphoreNew(1,0,NULL);

    osThreadNew(tBrain, NULL, NULL);    // Create application main thread
//	osThreadNew(tLED, NULL, NULL);
//	osThreadNew(tAudio, NULL, NULL);
    osThreadNew(tLed_red, NULL, NULL);
    osThreadNew(tLed_green, NULL, NULL);
	osThreadNew(tMotorControl, NULL, NULL);
    osKernelStart();                      // Start thread execution

}

