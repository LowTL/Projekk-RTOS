/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "MKL25Z4.h"                    // Device header
#include "progInit.h"

uint8_t UART2_Receive_Poll(void)
{
	//wait until receive data register is full
	while (!(UART2->S1 & UART_S1_RDRF_MASK));
	return (UART2->D);	
}
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
void app_main (void *argument) {
 
  // ...
  for (;;) {}
}

void tBrain (void *argument) {

}

void tMotorControl (void *argument) {

}

void tLED (void *argument) {

}

void tAudio (void *argument) {

}

void Serial_ISR (void *argument) {

}
 
int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initUART2();
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
  osThreadNew(tBrain, NULL, NULL);    // Create application main thread
	osThreadNew(tLED, NULL, NULL);
	osThreadNew(tAudio, NULL, NULL);
	osThreadNew(tMotorControl, NULL, NULL);
  osKernelStart();                      // Start thread execution
  for (;;) {}
}
