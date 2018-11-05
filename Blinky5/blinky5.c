// GPIO.c
// Runs on TM4C1294
// Initialize four GPIO pins as outputs.  Continually generate output to
// drive simulated stepper motor.
// Daniel Valvano
// April 3, 2014

/* This example accompanies the books
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Volume 1 Program 4.5

   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2 Example 2.2, Program 2.8, Figure 2.32

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// PN3 is an output to positive logic LED3
// PN2 is an output to positive logic LED2
// PN1 is an output to positive logic LED1
// PN0 is an output to positive logic LED0

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define LEDS                    (*((volatile uint32_t *)0x4006403C))
#define PJ0       (*((volatile uint32_t *)0x40060004))
#define PJ1       (*((volatile uint32_t *)0x40060008))
#define SWITCHES  (*((volatile uint32_t *)0x4006000C))
#define SW1_PRESSED             0x02  // value read from location SWITCHES when just SW1 is pressed
#define SW2_PRESSED             0x01  // value read from location SWITCHES when just SW2 is pressed
#define BOTH_PRESSED            0x00  // value read from location SWITCHES when both switches are pressed
#define NO_PRESSED              0x03  // value read from location SWITCHES when neither switch is pressed

void GPIO_Init(void){
                                   // activate clock for Port N
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;
                                   // allow time for clock to stabilize
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0){};
  GPIO_PORTN_DIR_R |= 0x0F;        // make PN3-0 out
  GPIO_PORTN_AFSEL_R &= ~0x0F;     // disable alt funct on PN3-0
  GPIO_PORTN_DEN_R |= 0x0F;        // enable digital I/O on PN3-0
                                   // configure PN3-0 as GPIO
  GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFF0000)+0x00000000;
  GPIO_PORTN_AMSEL_R &= ~0x0F;     // disable analog functionality on PN3-0
}
//------------PortJ_Init------------
// Initialize GPIO Port J for negative logic switches on PJ1-0
// as the Connected LaunchPad is wired.  Weak internal pull-up
// resistors are enabled.
// Input: none
// Output: none
void PortJ_Init(void){
                                   // activate clock for Port J
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;
                                   // allow time for clock to stabilize
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R8) == 0){};
  GPIO_PORTJ_AHB_DIR_R = 0;            // make PJ1-0 in (PJ1-0 built-in SW2-1)
  GPIO_PORTJ_AHB_AFSEL_R = 0;          // disable alt funct on PJ1-0
  GPIO_PORTJ_AHB_PUR_R = 0x03;         // enable pull-up on PJ1-0
  GPIO_PORTJ_AHB_DEN_R = 0x03;         // enable digital I/O on PJ1-0
  GPIO_PORTJ_AHB_PCTL_R = 0;           // configure PJ1-0 as GPIO
  GPIO_PORTJ_AHB_AMSEL_R = 0;          // disable analog functionality on PJ1-0
}

//------------PortJ_Input------------
// Read and return the status of the switches.
// Input: none
// Output: 0x02 if only Switch 1 is pressed
//         0x01 if only Switch 2 is pressed
//         0x00 if both switches are pressed
//         0x03 if no switches are pressed
uint32_t PortJ_Input(void){
  return (GPIO_PORTJ_AHB_DATA_R&0x03); // read PJ1-0 inputs
}

//void Switch_Init(void){
//                                    // 1) activate clock for Port A
//  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0;
//                                    // allow time for clock to stabilize
//  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R0) == 0){};
//                                    // 2) no need to unlock GPIO Port A
//  GPIO_PORTA_AMSEL_R &= ~0x20;      // 3) disable analog functionality on PA5
//  GPIO_PORTA_PCTL_R &= ~0x00F00000; // 4) configure PA5 as GPIO
//  GPIO_PORTA_DIR_R &= ~0x20;        // 5) make PA5 input
//  GPIO_PORTA_AFSEL_R &= ~0x20;      // 6) disable alt funct on PA5
//  GPIO_PORTA_DEN_R |= 0x20;         // 7) enable digital I/O on PA5
//}

void ledOn(int led)
{
	switch(led)
	{
		case 1:
			LEDS = 1;
			break;
		case 2:
			LEDS = 2;
			break;
		case 3:
			LEDS = 3;
			break;
}
	}

void allOff()
{
	LEDS=0;
}

void delay()
{
	uint32_t count = (1<<14);
	uint32_t i = 0;
	while (i != count) {
		i++;
	}
}


int main(void){
  GPIO_Init();
	//Switch_Init();
	PortJ_Init();
	int led = 0;
	int flag1 = 0;
	int flag2 = 0;
	int state1 = 0;
	int state2 = 0;
	//ledOn(2);
  while(1)
	{
		if(PortJ_Input() == SW2_PRESSED && flag1==0)
		{
			state1 ^= 0x01;
			if ((state2 | state1) > 0) {
				ledOn(state2 | state1);
			} else {
				allOff();
			}
			//allOff();
			flag1=1;
			
		}
		if(PortJ_Input() == SW1_PRESSED && flag2==0)
		{
			state2 ^= 0x02;
			if ((state2 | state1) > 0) {
				ledOn(state2 | state1);
			} else {
				allOff();
			}
			//allOff();
			flag2=1;
		}
		//delay();
		ledOn(led);
		//delay();
		if(PortJ_Input() == NO_PRESSED)
		{
			flag1=0;
			flag2=0;
		}			
		
		
  }
}
