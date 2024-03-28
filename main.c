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

osSemaphoreId_t uartSem;
osSemaphoreId_t motorMove; //currently unused


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
	if(input>>2) { //move fwd
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

	if(input>>2) { //move fwd
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
	}
}

void tMotorControl (void *argument) {

	for(;;) {
		osSemaphoreAcquire(uartSem,osWaitForever);
		input2 = input2 + 1;
		right_motor = (msg & 0b00000111);
		left_motor = (msg & 0b00111000) >> 3;
		rightMotor(right_motor);
		leftMotor(left_motor);
	}

}

void tLED (void *argument) {
	for(;;) {
	//	osSemaphoreAcquire(uartSem, osWaitForever);
		control_RGB_LEDs(0, true);
		osDelay(1000);
		control_RGB_LEDs(0, false);
		osDelay(1000);
//		osSemaphoreRelease(uartSem);

	}
}

void tAudio (void *argument) {

}

void Serial_ISR (void *argument) {

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
	motorMove = osSemaphoreNew(1,0,NULL);

  osThreadNew(tBrain, NULL, NULL);    // Create application main thread
	osThreadNew(tLED, NULL, NULL);
//	osThreadNew(tAudio, NULL, NULL);
	osThreadNew(tMotorControl, NULL, NULL);
  osKernelStart();                      // Start thread execution

}

