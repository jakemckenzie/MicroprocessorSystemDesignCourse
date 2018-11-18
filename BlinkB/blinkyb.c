#include <stdint.h>
#include <stdbool.h>
#include "tm4c1294ncpdt.h"
#include "sysctl.h"

#define LEDS                    (*((volatile uint32_t *)0x4006403C))
#define PJ1                     (*((volatile uint32_t *)0x40060008))
#define SW2                     0x01  // value read from location SWITCHES when just SW2 is pressed

//NVIC interrupt enableconfiguration
#define NVIC_EN1_INT51   0x00080000   // Interrupt 51 enable
// global state used inside interrupt
volatile uint32_t state = 0x00;

// Function prototypes
void GPIOPortJ_Handler(void);
void init_GPIO_N(void);
void init_GPIO_J(void);
void delay(void);
void turn_off_LEDS(void);
uint32_t input_from_Port_J(void);
void IntGlobalEnable(void);
void IntGlobalDisable(void);
void init_Interrupt_Port_J(void);

int main(void) {
    //Initialization of GPIO Port N,
    init_GPIO_N();
    //Initialization of GPIO Port J,
    init_GPIO_J();
	// stop the funny business
    IntGlobalDisable();
    // Initialization of Port J interrupts
    init_Interrupt_Port_J();
    // time for funny business
    IntGlobalEnable();
    // so that it doesn't end.
    while(1) {

    }
}



// Interrupt service routine for Port J
void GPIOPortJ_Handler (void) {
    LEDS ^= 0x01;
    GPIO_PORTJ_AHB_ICR_R ^= SW2;
}

/*
 * switch 1 Port J input states:
 * 
 * 0x00 if both switches are pressed.
 * 0x01 if switch 2 is pressed (so stupid TI f**k you)
 * 0x02 if switch 1 is pressed
 * 0x03 if both switch 1 and 2 are pressed 
 * 
 * @returns only the input pertinate to switch 1 and 2
 */
uint32_t input_from_Port_J(void) {
    return (GPIO_PORTJ_AHB_DATA_R & 0x03);
}

/*
 * Initialization of GPIO Port N, which are for the LEDS.
 */
void init_GPIO_N(void) {
    //activate clock for Port N
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;
    // allowing the clock to catch up
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0) {};
    // select PN3 through PN0
    GPIO_PORTN_DIR_R |= 0x0F;
    // disable alternative functions on PN3-0
    //GPIO_PORTN_AFSEL_R &= ~0x0F;
    // enable digital I/O on PN3-0
    GPIO_PORTN_DEN_R |= 0x0F;   
    // configure PN3-0 as GPIO with some added delay for the clock
    //GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R & 0xFFFF0000) + 0x00000000;
    // disable analog functionality on PN3-0
    //GPIO_PORTN_AMSEL_R &= ~0x0F;     
}
/*
 * Initialization of GPIO Port J, which are for the switiches. 
 */ 
void init_GPIO_J(void) {
    // activate clock for Port J
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R8;
    // allow time for clock to stabilize
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R8) == 0) {};
    // make PJ1-0 in (PJ1-0 built-in SW2-1)
    GPIO_PORTJ_AHB_DIR_R = 0;
    // disable alt funct on PJ1-0
    //GPIO_PORTJ_AHB_AFSEL_R = 0;
    // enable pull-up on PJ1-0
    GPIO_PORTJ_AHB_PUR_R = SW2;
    // enable digital I/O on PJ1-0         
    GPIO_PORTJ_AHB_DEN_R = SW2;
    // configure PJ1-0 as GPIO         
    GPIO_PORTJ_AHB_PCTL_R = 0;
    // disable analog functionality on PJ1-0           
    //GPIO_PORTJ_AHB_AMSEL_R = 0;                    
}
/*
 * Initialization of GPIO Port J Interrupt. 
 * 
 * Information found from Valvano Chapter 12 on interrupts:
 * 
 *  DIR      AFSEL   PMC         IS      IBE     IEV     IME    Port mode
 *  0        0       0000        0       0       0       1      Input, 
 *                                                              falling edge trigger, 
 *                                                              interrupt
 * 
 * ^ This is for a different board but it made a lot more sense than anything in 
 * those stupid DOCS by TI
 * 
 */
void init_Interrupt_Port_J(void) {
    // Interrupt INT 51 enabled in NVIC
    NVIC_EN1_R |= NVIC_EN1_INT51;
    // configure GPIOJ interrupt priority as 0
    NVIC_PRI12_R &= ~0xE0000000;
    // arm interrupt for PJ1 
    GPIO_PORTJ_AHB_IM_R |= SW2;
    // make PJ1 edge-sensitve
    GPIO_PORTJ_AHB_IS_R &= ~SW2;
    // disable both edge sentivity for PJ1
    GPIO_PORTJ_AHB_IBE_R &= ~SW2;
    // PJ1 falling edge event;
    GPIO_PORTJ_AHB_IEV_R &= ~SW2;
}

/*
 * Around the same delay you gave us in your blink example. 
 * I made it a power of 2 instead.
 */
void delay(void) {
    uint32_t count = (1<<17);
    uint32_t i = 0;
    while ((++i) != count) { };
}

/*
 * Globally enabled interrupts
 */
void IntGlobalEnable(void) {
    __asm("    cpsie   i\n");
}

/*
 * Globally disabled interrupts
 */
void IntGlobalDisable(void) {
    __asm("    cpsie   i\n");
}
