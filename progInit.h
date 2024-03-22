#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "system_MKL25Z4.h"             // Keil::Device:Startup
#include "MKL25Z4.h"                    // Device header
#include "constants.h"

void initUART2()
{
	uint32_t bus_clock, divisor;
	//enable clock to UART2 and PORTE
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	//connect UART pins for PORTE22, do for 23
	PORTE->PCR[22] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[22] |= PORT_PCR_MUX(4);
	PORTE->PCR[23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[23] |= PORT_PCR_MUX(4);
	
	PORTE->PCR[23] |= (0b1010 << 16);
	
	//ensure Tx and Rx disabled
	UART2->C2 &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_TIE_MASK | UART_C2_RIE_MASK);
	
	//set baud rate
	bus_clock = (DEFAULT_SYSTEM_CLOCK) / 2;
	divisor = bus_clock / (BAUD_RATE * 16);
	UART2_BDH = UART_BDH_SBR(divisor >> 8);
	UART2_BDL = UART_BDL_SBR(divisor);
	
	UART2->C1 = 0;
	UART2->C2 = 0;
	UART2->C3 = 0;
	
	//Clear & enable Interupts in UART2
	//Priority is set at 128
	NVIC_SetPriority(UART2_IRQn, 128);
	NVIC_ClearPendingIRQ(UART2_IRQn);
	NVIC_EnableIRQ(UART2_IRQn);
	
	//Enable Tx and Rx
	UART2->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
	//Tx/Rx Interrupt Enable
	UART2->C2 |= (UART_C2_TIE_MASK | UART_C2_RIE_MASK);
	
}

void initLED() {
	// Enable Clock to PORTB and PORTD
	SIM->SCGC5 |= ((SIM_SCGC5_PORTB_MASK) | (SIM_SCGC5_PORTD_MASK));
	// Configure MUX settings to make all 3 pins GPIO
	PORTB->PCR[RED_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED] |= PORT_PCR_MUX(1);
		
	PORTB->PCR[GREEN_LED] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED] |= PORT_PCR_MUX(1);
		
	PORTD->PCR[BLUE_LED] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED] |= PORT_PCR_MUX(1);
		
	// Set Data Direction Registers for PortB and PortD
	PTB->PDDR |= (MASK(RED_LED) | MASK(GREEN_LED));
	PTD->PDDR |= MASK(BLUE_LED);
	
	//turns everything off
	PTB->PSOR |= (1 << RED_LED | 1 << GREEN_LED);
	PTD->PSOR |= 1 << BLUE_LED;
}

void initPWM(void) 
{
	//enable power to clock
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	//Enable Port B 0, MUX set to 3 for ALT3 -> Timer 1 channel 0
	PORTB->PCR[0] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[0] |= PORT_PCR_MUX(3);
	
	//Enable Port B 1, MUX set to 3 for ALT3 -> Timer 1 channel 1
	PORTB->PCR[1] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[1] |= PORT_PCR_MUX(3);
	
	//power SCGC6 Timer Power Module 1
	SIM_SCGC6 |= SIM_SCGC6_TPM1_MASK;
	
	//Can set MOD value of 20971520/128 = 163840/MOD = 50 Hz
	//TPM1->MOD = 3276;
		
	// Select clock source
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	//Set MOD value 48000000/128 = 375000/MOD = 50Hz
	TPM1->MOD = 7500;
	
	//Set TPM1 CMOD = 01 and PS = 111, Prescalar = 128
	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	
	//set CPMWS to 0, counting up mode
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	//Set ELSB to 1 and MSB to 1, enable PWM on PTB0
	//Edge Aligned PWM, HIGH TRUE PULSES (Clear Output on compare match)
	TPM1_C0SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));

	//Set ELSB to 1 and MSB to 1, enable PWM on PTB1
	TPM1_C1SC &= ~(TPM_CnSC_ELSB_MASK | TPM_CnSC_ELSA_MASK | TPM_CnSC_MSB_MASK | TPM_CnSC_MSA_MASK);
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB(1));
}
