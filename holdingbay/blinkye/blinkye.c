#include <stdint.h>
#include <stdbool.h>
#include "tm4c1294ncpdt.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "sysctl.h"
#include "gpio.h"
#include "interrupt.h"
#include "timer.h"
// // Timer 1 run mode clock control
// //#define SYSCTL_RCGC_TIMER_R (*((volatile uint32_t *)0x))
// // General purpose timer register definitions
// #define TIMER1_CFG_R (*((volatile uint32_t *)0x))
// #define TIMER1_TAMR_R (*((volatile uint32_t *)0x))
// #define TIMER1_CTL_R (*((volatile uint32_t *)0x))
// #define TIMER1_IMR_R (*((volatile uint32_t *)0x))
// #define TIMER1_ICR_R (*((volatile uint32_t *)0x))
// #define TIMER1_TAILR_R (*((volatile uint32_t *)0x))
// #define TIMER1_TAPR_R (*((volatile uint32_t *)0x))
// // IRQ 0 to 31 enable register
// #define NVIC_EN0_R (*((volatile uint32_t *)0x))
// // Timer1 A interrupt is assigned to NVIC IRQ21
// #define NVIC_EN0_INT21 0x
// // Assign Interrupt Priority to Timer1 A
// #define NVIC_PRI?_R (*((volatile uint32_t *)0x))
// // Timer1 A bit field definitions for mode configuration
// #define TIM_16_BIT_CONFIG 0x // 16- bit timer
// #define TIM_PERIODIC_MODE 0x // Timer mode
// #define TIM_A_ENABLE 0x // Timer A enable control
// // Timer1 A time out event interrupt mask and clear
// #define TIM_A_TIMEOUT_IM 0x // Interrupt mask
#define TIM_A_TIMEOUT_IC 0x // Interrupt clear
// Timer1 A reload & prescale values for 10 ms interval
// if the required interval is 1 second, both the reload value
// and the pre-scale need to be recalculated and decided
#define TIM_A_INTERVAL 0x2710  // Timer reload value for
// 10 ms interval when system
// clock frequency is 16 MHz
// (1/(16*100) ) *16 ,000 ,000
// = 10,000 = 0x2710
#define TIM_A_PRESCALE 0x0F // 1/16 prescaler
int main ( void ){
    // Configure Timer1 A periodic timer with timeout event interrupt
    SYSCTL_RCGCTIMER_R ??? ; // Enable clock for timer 1
    TIMER1_CFG_R ??? TIM_16_BIT_CONFIG ; // Configure 16- bit timer
    TIMER1_TAMR_R ??? TIM_PERIODIC_MODE ;// Periodic count down
    // Configure the timer reload value
    TIMER1_TAILR_R = TIM_A_INTERVAL ;
    TIMER1_TAPR_R = TIM_A_PRESCALE ;
    // Timer1 A interrupt configuration
    TIMER1_IMR_R ??? TIM_A_TIMEOUT_IM ;
    NVIC_EN0_R = NVIC_EN0_INT21 ; // Enable INT 21 in NVIC
    // Enable the timer
    TIMER1_CTL_R ??? TIM_A_ENABLE ;

    for (;;) {
        
    }
}
void Timer1A_Handler ( void ){
    // Clear interrupt flag
    TIMER1_ICR_R ??? TIM_A_TIMEOUT_IC;
    // Perform other necessary tasks
}