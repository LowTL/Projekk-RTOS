#include "MKL25Z4.h"            // Device header

#define LEFT_MOTOR_FWD 1        //PTC1 ALT4 - TPM0_CH0
#define LEFT_MOTOR_RVS 2        //PTC2 ALT4 - TPM0_CH1
#define RIGHT_MOTOR_FWD 29      //PTE29 ALT3 - TPM0_CH2
#define RIGHT_MOTOR_RVS 30      //PTE30 ALT3 - TPM0_CH3

#define TURN_MULTIPLIER 0.8     // Multiplier to slow down motor when turning

void initMotors(void)
{
	//Enable clock for Port C and E
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;

	// Configure PIN MUX
	PORTC->PCR[LEFT_MOTOR_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[LEFT_MOTOR_FWD] |= PORT_PCR_MUX(4);

	PORTC->PCR[LEFT_MOTOR_RVS] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[LEFT_MOTOR_RVS] |= PORT_PCR_MUX(4);

	PORTE->PCR[RIGHT_MOTOR_FWD] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[RIGHT_MOTOR_FWD] |= PORT_PCR_MUX(3);

	PORTE->PCR[RIGHT_MOTOR_RVS] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[RIGHT_MOTOR_RVS] |= PORT_PCR_MUX(3);

	//Enable clock for TPM0 module
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

	//Select MCGFLLCLK for system
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	//Set timer to 50Hz
	TPM0->MOD = 7500;

	//Set LPTPM counter to increment on every counter clock
	//Set prescaler to 128
	//Select up-counting mode
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0->SC |= ((TPM_SC_CMOD(1)) | (TPM_SC_PS(7)));
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);

	//Select edge-aligned PWM with high-true pulses, for TPM0_CH0, TPM0_CH1, TPM0_CH2, TPM0_CH3
	TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C0SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
	TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C1SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
	TPM0_C2SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C2SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
	TPM0_C3SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C3SC |= (TPM_CnSC_ELSB(1)) | (TPM_CnSC_MSB(1));
}