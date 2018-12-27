// Alex Marlow & Jake McKenzie & Ammon Dodson
//external temperature sensor program

//libraries included for system clock call
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"


// providing clock to GPIO module
#define SYSCTL_RCGCGPIO_R       (*((volatile unsigned long *)0x400FE608))
#define SYSCTL_RCGCGPIO_GPIOD 0x00000008; //enable for port D

// providing clock to ADC module
#define SYSCTL_RCGCADC_R        (*((volatile unsigned long *)0x400FE638))
#define SYSCTL_RCGCADC_ADC0 0x01;   //enables ADC0

// ADC0 and SS3
#define ADC0_ACTSS_R            (*((volatile unsigned long *)0x40038000))    //enables sequence
#define ADC0_IM_R               (*((volatile unsigned long *)0x40038008))	//enables interrupts
#define ADC0_ISC_R              (*((volatile unsigned long *)0x4003800C))	//clears interrupt
#define ADC0_EMUX_R             (*((volatile unsigned long *)0x40038014))   //EMUX selects trigger
#define ADC0_PSSI_R             (*((volatile unsigned long *)0x40038028))   //starts sequence
#define ADC0_SAC_R              (*((volatile unsigned long *)0x40038030))    //average register
#define ADC0_SSMUX3_R           (*((volatile unsigned long *)0x400380A0))    //selects AIN13
#define ADC0_SSCTL3_R           (*((volatile unsigned long *)0x400380A4))	//sets interrupts to happen at a certain time
#define ADC0_SSFIFO3_R          (*((volatile unsigned long *)0x400380A8))   //read this for averaged value
#define ADC0_PC_R               (*((volatile unsigned long *)0x40038FC4))    //selects wait value for after average

#define ACTSS_SS3_ENABLE 0x8;  //enables sequence 3 standard
#define IM_SS3_ENABLE 0x00000008;    //enables interrupts on sequencer 3
#define ISC_SS3_CLEAR 0x8;    //clears interrupt on sequencer 3
#define EMUX_SS3_DEFUALT 0xF000;     //makes the EMUX us its constant trigger mode
#define PSSI_SS3_START 0x00000008;   //starts the sequencer
#define SAC_16X_SET 0x00000004;     //sets the averaging to 16x
#define SSMUX3_AIN13_SET 0xD;    //selects AIN13 as analog input
#define SSCTL3_INTEND 0x6;     //sets interrupt to happen at end of sequence and makes it stop
                                //read the SSFIFO3 to get the sequenced averaged data value
#define PC_WAIT48 0x3;          //gives a pause of 48 after a sequence


// GPIO port D
#define GPIO_PORTD_AHB_DIR_R    (*((volatile unsigned long *)0x4005B400))   //Direction select register
#define GPIO_PORTD_AHB_DEN_R    (*((volatile unsigned long *)0x4005B51C))   //digital on off register
#define GPIO_PORTD_AHB_AMSEL_R  (*((volatile unsigned long *)0x4005B528))     //for turning on analog function

#define GPIO_PORTD_AHB_AFSEL_R  (*((volatile unsigned long *)0x4005B420))    //alternate function register

#define PORTD_DIR 0x04;  //set direction for D
#define POTD_DEN 0x00;   //turn digital enable off by anding
#define PORTD_AMSEL 0x04;  //enables analog for D2 and disables isolation and digital for D2
#define PORTD_AFSEL 0x04;  //set alternate function


// NVIC interrupt module
#define NVIC_EN0_R              (*((volatile unsigned long *)0xE000E100))

#define NVIC_SS3 0x00020000;    //enables interrupt 17 for SS3

int Temperature = 0;

unsigned long ui32SysClkFreq;  //used for clock frequency


// This function generates the delay
void Delay ( volatile unsigned int delay ){

	volatile unsigned int i, j;
	for (i = 0; i < delay ; i++) {
		// introduces a delay of about 10 us at 16 MHz
		for (j = 0; j < 12; j ++);
	}
}

//integer for data input
int DATA = 0;

// ADC0 interrupt service routine for sequencer 3
void ADC0SS3_Handler (void) {

	ADC0_ISC_R |= ISC_SS3_CLEAR;   //clear interrupt

	DATA = ((ADC0_SSFIFO3_R) & 0x0FFF);     //read data in input data
	Temperature = (((DATA*.806) - 750)/10 + 25);		//do conversions

	ADC0_ISC_R |= ISC_SS3_CLEAR;			//clear interrupt again, (for some reason this had to be done)
}


// main function
int main ( void ){


	//sets main clock frequency
	ui32SysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	// Enable the clock to port D
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_GPIOD;

	// configuration of port D pin2
	GPIO_PORTD_AHB_DIR_R |= PORTD_DIR;       //input set
	GPIO_PORTD_AHB_DEN_R &= POTD_DEN;			//digital enable off
	GPIO_PORTD_AHB_AMSEL_R |= PORTD_AMSEL;		//set alternate function registers
	GPIO_PORTD_AHB_AFSEL_R |= PORTD_AFSEL;

	// Enable the clock for ADC0
	SYSCTL_RCGCADC_R |= SYSCTL_RCGCADC_ADC0;


	//turns off ADC0 for configuration
	ADC0_ACTSS_R |= 0x0;

	// delay
	Delay (3);
	//sets continuous triggering
	ADC0_EMUX_R |= EMUX_SS3_DEFUALT;

	// Select AN13 ( PD2 ) as the analog input
	ADC0_SSMUX3_R |= SSMUX3_AIN13_SET;


	// quarter conversion rate; 48*Tadc periods pause
	ADC0_PC_R |= PC_WAIT48;

	// 1st sample is end of sequence and source of interrupt
	ADC0_SSCTL3_R |=  SSCTL3_INTEND;

	// 16x oversampling and then averaged
	ADC0_SAC_R |= SAC_16X_SET;

	// Unmask ADC0 sequence 3 interrupt
	ADC0_IM_R |= IM_SS3_ENABLE;

	// Clear the interrupt for ADC0 sequencer 3
	ADC0_ISC_R |= ISC_SS3_CLEAR;

	// Enable ADC0 sequencer 3 interrupt in NVIC
	NVIC_EN0_R |= NVIC_SS3;


	// Enable ADC0 module for sequencer 3
	ADC0_ACTSS_R |= ACTSS_SS3_ENABLE;

	// Initiate sequencer 3
	ADC0_PSSI_R |= PSSI_SS3_START;

	// wait certain time for ADC module do the conversion

	Delay(100);
	//loop for program to continue running
	while (1) {
	}

}
