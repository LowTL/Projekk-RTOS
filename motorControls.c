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

int main(void) {
	return 0;
}

//--------------------------------------------------------------------------------
// EXPLANATION
/*
	PWM set to up-counting mode, edge-aligned PWM with high-true pulses.
	1. (counter counts from 0 to MOD-1 and repeat)
	2. (output clears to 0 when counter matches CnV value, output sets to 1 when counter reloads)

	PWM MOD controls frequency of PWM (does not matter for motor control)
	PWM CnV controls duty cycle of PWM (affects speed of motors)

	C0V and C1V handles the forward/reverse direction of both left motors
	C2V and C3V handles the forward/reverse direction of both right motors

	C0V = IN1/IN3 [LEFT MOTORS FWD]
	C1V = IN2/IN4 [LEFT MOTORS RVS]
	C2V = IN1/IN3 [RIGHT MOTORS FWD]
	C3V = IN2/IN4 [RIGHT MOTORS RVS]

	IN1/IN3	IN2/IN4	Spinning Direction
	Low(0)	Low(0)	Motor OFF
	High(1)	Low(0)	Forward
	Low(0)	High(1)	Reverse
	High(1)	High(1)	Motor OFF

SIMPLIFED MOTOR CONTROLS: 
	0 means Low
	1 means High
	Weaker means the value should be set lower (for turning purposes)

	Stop:  		Left:     Right:   
	C0V = 0;	C0V = 0;  C0V = 1;  
	C1V = 0;	C1V = 1;  C1V = 0;  
	C2V = 0;	C2V = 1;  C2V = 0; 
	C3V = 0;	C3V = 0;  C3V = 1; 

	Forward:  ForwardLeft:     ForwardRight:
	C0V = 1;  C0V = 1; Weaker  C0V = 1;
	C1V = 0;  C1V = 0; Weaker  C1V = 0;
	C2V = 1;  C2V = 1;		   C2V = 1; Weaker
	C3V = 0;  C3V = 0;         C3V = 0; Weaker

	Backward: BackwardLeft:    BackwardRight:
	C0V = 0;  C0V = 0; Weaker  C0V = 0;
	C1V = 1;  C1V = 1; Weaker  C1V = 1;
	C2V = 0;  C2V = 0;         C2V = 0; Weaker
	C3V = 1;  C3V = 1;         C3V = 1; Weaker
*/

//--------------------------------------------------------------------------------------------------------------------------------
// MOTOR CONTROLS IN MULTIPLE FUNCTIONS
// STOP, FORWARD, FORWARDLEFT, FORWARDRIGHT, BACKWARD, BACKWARDLEFT, BACKWARDRIGHT, LEFT, RIGHT
//--------------------------------------------------------------------------------------------------------------------------------

void stop(void)
{
	TPM0_C0V = 0;
	TPM0_C1V = 0;
	TPM0_C2V = 0;
	TPM0_C3V = 0;
}

void moveForward(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float dutyCycle = (float) power / 100.0f;
	
	TPM0_C0V = TPM0->MOD * dutyCycle;
	TPM0_C1V = 0;
	TPM0_C2V = TPM0->MOD * dutyCycle;
	TPM0_C3V = 0;
}

void moveBackward(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float dutyCycle = (float) power / 100.0f;
	
	TPM0_C0V = 0;
	TPM0_C1V = TPM0->MOD * dutyCycle;
	TPM0_C2V = 0;
	TPM0_C3V = TPM0->MOD * dutyCycle;
}

void turnLeft(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float dutyCycle = (float) power / 100.0f;
	
	TPM0_C0V = 0;
	TPM0_C1V = TPM0->MOD * dutyCycle;
	TPM0_C2V = TPM0->MOD * dutyCycle;
	TPM0_C3V = 0;
}

void turnRight(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float dutyCycle = (float) power / 100.0f;
	
	TPM0_C0V = TPM0->MOD * dutyCycle;
	TPM0_C1V = 0;
	TPM0_C2V = 0;
	TPM0_C3V = TPM0->MOD * dutyCycle;
}

void moveForwardLeft(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float dutyCycle = (float) power / 100.0f;
	
	TPM0_C0V = TPM0->MOD * dutyCycle * TURN_MULTIPLIER; // Slow down left motors
	TPM0_C1V = 0;
	TPM0_C2V = TPM0->MOD * dutyCycle;
	TPM0_C3V = 0;
}

void moveForwardRight(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float dutyCycle = (float) power / 100.0f;
	
	TPM0_C0V = TPM0->MOD * dutyCycle;
	TPM0_C1V = 0;
	TPM0_C2V = TPM0->MOD * dutyCycle * TURN_MULTIPLIER; // Slow down right motors
	TPM0_C3V = 0;
}

void moveBackwardLeft(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float dutyCycle = (float) power / 100.0f;
	
	TPM0_C0V = 0;
	TPM0_C1V = TPM0->MOD * dutyCycle * TURN_MULTIPLIER; // Slow down left motors
	TPM0_C2V = 0; 
	TPM0_C3V = TPM0->MOD * dutyCycle; 
}

void moveBackwardRight(int power)
{
	if (power > 100)
		power = 100;
	if (power < 0)
		power = 0;
	
	float dutyCycle = (float) power / 100.0f;
	
	TPM0_C0V = 0;
	TPM0_C1V = TPM0->MOD * dutyCycle;
	TPM0_C2V = 0; 
	TPM0_C3V = TPM0->MOD * dutyCycle * TURN_MULTIPLIER; // Slow down right motors
}

//--------------------------------------------------------------------------------------------------------------------------------
// MOTOR CONTROLS IN A SINGLE FUNCTION
//--------------------------------------------------------------------------------------------------------------------------------

enum LateralMovement 
{
	NONE = 0,
	FORWARD = 1,
	BACKWARD = 2
};
 
enum Turning 
{
	NO_TURN = 0,
  	LEFT = 1,
  	RIGHT = 2
};

/* 
	Create const float arrays[3][3] to hold lateral and turn states for 
	TPM0_C0V, TPM0_C1V, TPM0_C2V, TPM0_C3V 
*/

// Duty Cycle multiplier for TPM0_C0V
const float control_TPM0_C0V[3][3] = 
{
  // NONE movement
  {0, 0, 1.0},    	 // NO_TURN, LEFT, RIGHT 
  // FORWARD movement
  {1.0, TURN_MULTIPLIER, 1.0},  // NO_TURN, LEFT, RIGHT
  // BACKWARD movement 
  {0, 0, 0},       // NO_TURN, LEFT, RIGHT
};

// Duty Cycle multiplier for TPM0_C1V
const float control_TPM0_C1V[3][3] = 
{
  // NONE movement
  {0, 1.0, 0},        // NO_TURN, LEFT, RIGHT 
  // FORWARD movement
  {0, 0, 0},         // NO_TURN, LEFT, RIGHT
  // BACKWARD movement 
  {1.0, TURN_MULTIPLIER, 1.0}  // NO_TURN, LEFT, RIGHT
};

// Duty Cycle multiplier for TPM0_C2V
const float control_TPM0_C2V[3][3] = 
{
	// NONE movement
  {0, 1.0, 0},         // NO_TURN, LEFT, RIGHT 
  // FORWARD movement
  {1.0, 1.0, TURN_MULTIPLIER},   // NO_TURN, LEFT, RIGHT
  // BACKWARD movement 
  {0, 0, 0}       // NO_TURN, LEFT, RIGHT
};

// Duty Cycle multiplier for TPM0_C3V
const float control_TPM0_C3V[3][3] = 
{
  // NONE movement
  {0, 0, 1.0},         // NO_TURN, LEFT, RIGHT 
  // FORWARD movement
  {0, 0, 0},         // NO_TURN, LEFT, RIGHT
  // BACKWARD movement 
  {1.0, 1.0, TURN_MULTIPLIER}, // NO_TURN, LEFT, RIGHT
};

void motorControl(enum LateralMovement lat, enum Turning turn, uint8_t power) 
{
	// Unsigned Int can never go below 0
	if (power > 100) {
		power = 100;
	} 
	
	float dutyCycle = (float) power / 100.0f;
	
  TPM0_C0V = TPM0->MOD * dutyCycle * control_TPM0_C0V[lat][turn];
  TPM0_C1V = TPM0->MOD * dutyCycle * control_TPM0_C1V[lat][turn];
  TPM0_C2V = TPM0->MOD * dutyCycle * control_TPM0_C2V[lat][turn];
  TPM0_C3V = TPM0->MOD * dutyCycle * control_TPM0_C3V[lat][turn];
}
