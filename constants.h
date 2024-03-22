#define RED_LED 18 // PortB Pin 18
#define GREEN_LED 19 // PortB Pin 19
#define BLUE_LED 1 // PortD Pin 1
#define MASK(x) (1 << (x))

#define BAUD_RATE 128000

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "MKL25Z4.h"                    // Device header
#include "stdio.h"
#include "stdbool.h"

osSemaphoreId_t uartSem;
