/*
 * Jake McKenzie Lab 4 Blinky with 2 switches and 2 LEDs
 * 
 * The point of this laboratory assignment is to get 
 * LED (PN1 and PN0) to fire when switches (PJ0 and PJ1)
 * are pressed. 
 */

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define LEDS                    (*((volatile uint32_t *)0x4006403C))
#define PJ0                     (*((volatile uint32_t *)0x40060004))
#define PJ1                     (*((volatile uint32_t *)0x40060008))
#define SWITCHES                (*((volatile uint32_t *)0x4006000C))
#define SW1_PRESSED             0x02  // value read from location SWITCHES when just SW1 is pressed
#define SW2_PRESSED             0x01  // value read from location SWITCHES when just SW2 is pressed
#define BOTH_PRESSED            0x00  // value read from location SWITCHES when both switches are pressed
#define NO_PRESSED              0x03  // value read from location SWITCHES when neither switch is pressed

/*
 * Initialization of GPIO Port N, which are for the LEDS.
 */
void init_GPIO_N(void){
    //activate clock for Port N
    GPIO_PORTN_DIR_R |= SYSCTL_RCGCGPIO_R12;
    // allowing the clock to catch up
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0) {};
    // select PN3 through PN0
    GPIO_PORTN_DIR_R |= 0x0F;
    // disable alternative functions on PN3-0/*
 * Jake McKenzie Lab 4 Blinky with 2 switches and 2 LEDs
 * 
 * The point of this laboratory assignment is to get 
 * LED (PN1 and PN0) to fire when switches (PJ0 and PJ1)
 * are pressed. 
 */

#include <stdint.h>
#include "tm4c1294ncpdt.h"

#define LEDS                    (*((volatile uint32_t *)0x4006403C))
#define PJ0                     (*((volatile uint32_t *)0x40060004))
#define PJ1                     (*((volatile uint32_t *)0x40060008))
#define SWITCHES                (*((volatile uint32_t *)0x4006000C))
#define SW1_PRESSED             0x02  // value read from location SWITCHES when just SW1 is pressed
#define SW2_PRESSED             0x01  // value read from location SWITCHES when just SW2 is pressed
#define BOTH_PRESSED            0x00  // value read from location SWITCHES when both switches are pressed
#define NO_PRESSED              0x03  // value read from location SWITCHES when neither switch is pressed

/*
 * Initialization of GPIO Port N, which are for the LEDS.
 */
void init_GPIO_N(void){
    //activate clock for Port N
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;
    // allowing the clock to catch up
    while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R12) == 0) {};
    // select PN3 through PN0
    GPIO_PORTN_DIR_R |= 0x0F;
    // disable alternative functions on PN3-0
    GPIO_PORTN_AFSEL_R &= ~0x0F;
    // enable digital I/O on PN3-0
    GPIO_PORTN_DEN_R |= 0x0F;   
    // configure PN3-0 as GPIO with some added delay for the clock
    GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R & 0xFFFF0000) + 0x00000000;
    // disable analog functionality on PN3-0
    GPIO_PORTN_AMSEL_R &= ~0x0F;     
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
    GPIO_PORTJ_AHB_AFSEL_R = 0;
    // enable pull-up on PJ1-0
    GPIO_PORTJ_AHB_PUR_R = 0x03;
    // enable digital I/O on PJ1-0         
    GPIO_PORTJ_AHB_DEN_R = 0x03;
    // configure PJ1-0 as GPIO         
    GPIO_PORTJ_AHB_PCTL_R = 0;
    // disable analog functionality on PJ1-0           
    GPIO_PORTJ_AHB_AMSEL_R = 0;                    
}
/*
 * switch 1 Port J input states:
 * 
 * 0x00 if both switches are pressed.
 * 0x01 if switch 2 is pressed (so stupid TI f**k you)
 * 0x02 if switch 1 is pressed
 * 0x03 if both switch 1 and 2 are pressed 
 */
uint32_t input_from_Port_J(void) {
    return (GPIO_PORTJ_AHB_DATA_R & 0x03);
}

void LED_state_machine(int led_id) {
    switch(led_id) {
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
/*
 * simple function for turning off the leds
 */
void turn_off_LEDS(void) {
    LEDS = 0x00;
}
/*
 * Around the same delay you gave us in your blink example. 
 * I made it a power of 2 instead.
 */
void delay(void) {
    uint32_t count = (1<<14);
    uint32_t i = 0;
    while ((++i) != count) { };
}

int main(void) {
    //Initialization of GPIO Port N,
    init_GPIO_N();
    //Initialization of GPIO Port J,
    init_GPIO_J();
		// initialize LED_state press 1
		uint32_t state_1 = 0x00;
		// initialize LED_state press 2
		uint32_t state_2 = 0x00;
    // initialize flag 1 to control flow to zero
    uint32_t flag1 = 0x00;
    // initialize flag 2 to control flow to zero
    uint32_t flag2 = 0x00;
		// initialize LEDS to off
		turn_off_LEDS();
    while (1) {
        if ((input_from_Port_J() == SW1_PRESSED) && (flag1 == 0)) {
            delay();
						// TI IS SO STUPID WHY WOULD YOU MAKE LED 1 0x02 and LED 2 0x01
						state_1 ^= 0x02;
						if ((state_1 | state_2 ) > 0) {
							LED_state_machine(state_1 | state_2);
						} else {
							turn_off_LEDS();
						}
            flag1 = 1;
        } 
        if ((input_from_Port_J() == SW2_PRESSED) && (flag2 == 0)) {
            delay();
						state_2 ^= 0x01;
						if ((state_1 | state_2 ) > 0) {
							LED_state_machine(state_1 | state_2);
						} else {
							turn_off_LEDS();
						}
            flag2 = 1;
        }
				delay();
        if(input_from_Port_J() == NO_PRESSED) {
            flag1 = 0;
            flag2 = 0;
        }

    }
}

    GPIO_PORTN_AFSEL_R &= ~0x0F;
    // enable digital I/O on PN3-0
    GPIO_PORTN_DEN_R |= 0x0F;   
    // configure PN3-0 as GPIO with some added delay for the clock
    GPIO_PORTN_PCTL_R = (GPIO_PORTN_PCTL_R&0xFFFF0000)+0x00000000;
    // disable analog functionality on PN3-0
    GPIO_PORTN_AMSEL_R &= ~0x0F;     
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
    GPIO_PORTJ_AHB_AFSEL_R = 0;
    // enable pull-up on PJ1-0
    GPIO_PORTJ_AHB_PUR_R = 0x03;
    // enable digital I/O on PJ1-0         
    GPIO_PORTJ_AHB_DEN_R = 0x03;
    // configure PJ1-0 as GPIO         
    GPIO_PORTJ_AHB_PCTL_R = 0;
    // disable analog functionality on PJ1-0           
    GPIO_PORTJ_AHB_AMSEL_R = 0;                    
}
/*
 * switch 1 Port J input states:
 * 
 * 0x00 if both switches are pressed.
 * 0x01 if switch 2 is pressed (so stupid TI f**k you)
 * 0x02 if switch 1 is pressed
 * 0x03 if both switch 1 and 2 are pressed 
 */
uint32_t input_from_Port_J(void) {
    return (GPIO_PORTJ_AHB_DATA_R & 0x03);
}

void LED_state_machine(int led_id) {
    switch(led_id) {
        case 0x01:
            LEDS = 0x01;
            break;
        case 0x02:
            LEDS = 0x02;
            break;
        case 0x03:
            LEDS = 0x04;
            break;
    }
}
/*
 * simple function for turning off the leds
 */
void turn_off_LEDS(void) {
    LEDS = 0x00;
}
/*
 * Around the same delay you gave us in your blink example. 
 * I made it a power of 2 instead.
 */
void delay(void) {
    uint32_t count = (1<<14);
    uint32_t i = 0;
    while ((++i) != count) { };
}

int main(void) {
    //Initialization of GPIO Port N,
    init_GPIO_N();
    //Initialization of GPIO Port J,
    init_GPIO_J();
		// initialize LED_state press 1
		uint32_t state_1 = 0x00;
		// initialize LED_state press 2
		uint32_t state_2 = 0x00;
    //initialize flag 1 to control flow to zero
    uint32_t flag1 = 0x00;
    //initialize flag 2 to control flow to zero
    uint32_t flag2 = 0x00;
    while (1) {
        if ((input_from_Port_J() == SW1_PRESSED) && (flag1 == 0)) {
            //delay();
						state_1 ^= 0x01;
            flag1 = 1;
        } 
        if ((input_from_Port_J() == SW2_PRESSED) && (flag2 == 0)) {
            //delay();
						state_2 ^= 0x02;
            flag2 = 1;
        }
				LED_state_machine(state_1 | state_2);
        //delay();
        if(input_from_Port_J() == NO_PRESSED) {
            flag1 = 0;
            flag2 = 0;
        }

    }
}
