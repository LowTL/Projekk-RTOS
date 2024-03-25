/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
#include "constants.h"
#include "progInit.h"
#include "ledControl.h"
#include "buzzerControl.h"
#include "motorControls.c"

volatile uint8_t msg;
volatile Q_T uartQ;

typedef struct {
	uint8_t cmd;
	uint8_t data;
} msgPkt;

uint8_t UART2_Receive_Poll(void)
{
	//wait until receive data register is full
	while (!(UART2->S1 & UART_S1_RDRF_MASK));
	return (UART2->D);
}

void UART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(UART2_IRQn);
	if ((UART2->S1 & UART_S1_RDRF_MASK)) {
	    Q_Enqueue(&uartQ, UART2 -> D);
		osSemaphoreRelease(uartSem);
	}

}

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
  for (;;) {}
}

void tBrain (void *argument) {
    osSemaphoreAcquire(uartSem, osWaitForever);
    msg = Q_Dequeue(&uartQ)
}

void tMotorControl (void *argument) {
    for(;;){
        osSemaphoreAcquire(motorSem, osWaitForever);

        enum LongiMovement move = msg % 4;
        enum Turning turn = (msg >> 2) % 4;

        if (msg % 4 == 3) move = 0;
        if ((msg >> 2) % 4 == 3) turn = 0; // handle invalid inputs

        motorControl(enum LongiMovement move, enum Turning turn, uint8_t power);
    }
}

void tLED (void *argument) {
    for(;;){
        uint8_t input = msg;
        if (input == 0x10) {
            control_RGB_LEDs(0, true);
        }
        osDelay(100);
        control_RGB_LEDs(0, false);
        osDelay(100);
        input = 0;
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
	initPWM();
	Q_Init(&uartQ);
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(tBrain, NULL, NULL);    // Create application main thread
	osThreadNew(tLED, NULL, NULL);
	osThreadNew(tAudio, NULL, NULL);
	osThreadNew(tMotorControl, NULL, NULL);
	uartSem = osSemaphoreNew(1,1,NULL);
	motorSem = osSemaphoreNew(1, 1, NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
