

#define LEDS (*((volatile uint32_t *)0x4006403C))
#define PJ0 (*((volatile uint32_t *)0x40060004))
#define PJ1 (*((volatile uint32_t *)0x40060008))
#define SWITCHES (*((volatile uint32_t *)0x4006000C))
//#define SYSCTL_RCGC2_GPIOF 0x00000020
//#define GPIO_PORTF_PIN4 0x10
//#define GPIO_PORTF_AHB_AFSEL_DIS 0x00 //disable alternate function
#define SW1_PRESSED 0x02              // value read from location SWITCHES when just SW1 is pressed
#define SW2_PRESSED 0x01              // value read from location SWITCHES when just SW2 is pressed
#define BOTH_PRESSED 0x00             // value read from location SWITCHES when both switches are pressed
#define NO_PRESSED 0x03               // value read from location SWITCHES when neither switch is pressed

void init_GPIO_N(void);
void init_GPIO_J(void);
uint32_t input_from_Port_J(void);
/*
 * Initialization of GPIO Port N, which are for the LEDS.
 */
void init_GPIO_N(void) {
    //activate clock for Port N
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;
    // allowing the clock to catch up
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R12) == 0) { };
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
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R8) == 0) { };
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
 * 
 * @returns only the input pertinate to switch 1 and 2
 */
uint32_t input_from_Port_J(void) {
    return (GPIO_PORTJ_AHB_DATA_R & 0x03);
}