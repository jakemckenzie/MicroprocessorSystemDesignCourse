// ------------------------Blink LED using Timers and Interrupts--------------------- //
/* LED on PN1, PN0, PF4, PF0
 * PF4 - LED 3
 * PN0 - LED 2
 * Switch - PJ0 and PJ1
 * BLINK LED 2 - PN0
 */


#include <stdint.h>
#include <stdbool.h>
#include "tm4c1294ncpdt.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "sysctl.h"
#include "gpio.h"
#include "interrupt.h"
#include "timer.h"

void initializeLED(void);
void initializeSwitch(void);
void setupClock(void);
void configureTimer(void);

int main(void) {
	
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_25MHZ|SYSCTL_OSC_MAIN);

	initializeLED();
	configureTimer();

	while(1){
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0, GPIO_PIN_0);
	}
}


void initializeLED(){

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
		while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION)){
		}

		GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
		GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_2);
}

void configureTimer(){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	TimerConfigure(TIMER1_BASE, TIMER_CFG_PERIODIC);

	uint32_t periods;
	periods = SysCtlClockGet()/10;
	TimerLoadSet(TIMER1_BASE, TIMER_A, periods);

	IntEnable(INT_TIMER1A);
	TimerIntEnable(TIMER1_BASE, TIMER_TIMA_TIMEOUT); //timeout interrupt
	IntMasterEnable();

	TimerEnable(TIMER1_BASE, TIMER_A);
}

void Timer1IntHandler(void){
	TimerIntClear(TIMER1_BASE, TIMER_TIMA_TIMEOUT);

	if(GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2)){
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_PIN_2);
	}else{
		GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_2, ~(GPIO_PIN_2));
}
}
