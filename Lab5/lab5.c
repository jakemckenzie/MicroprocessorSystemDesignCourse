#include <stdint.h>
#include <stdbool.h>
#include "tm4c1294ncpdt.h"
#include "hw_memmap.h"
#include "hw_types.h"
#include "sysctl.h"
#include "interrupt.h"
#include "gpio.h"
#include "timer.h"

int main(void) {
    uint32_t ui32Period;
    uint32_t ui32SysClkFreq;
    ui32SysClkFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ui32Period = ui32SysClkFreq/2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, (ui32Period - 1));
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);
    while(1) {

    }
}
void Timer0IntHandler(void) {
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Read the current state of the GPIO pin and
    // write back the opposite state
    if(GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_1)) {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 0);
    } else {
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1, 2);
    }
}
