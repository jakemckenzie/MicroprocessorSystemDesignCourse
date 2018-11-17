// Enable Port J&N clock control
#define SYSCTL_RCGCGPIO_R (*((volatile unsigned long *) 0x400FE608))
#define SYSCTL_RCGCGPIO_GPIOJN 0x00001100

// GPIO Port N, pin 1
#define GPIO_PORTN_DIR_R   (*((volatile unsigned long *) 0x40064400))
#define GPIO_PORTN_DEN_R   (*((volatile unsigned long *) 0x4006451C))
#define GPIO_PORTN_DATA_R (*((volatile unsigned long *) 0x400643FC))

// GPIO Port J, pin 1
#define GPIO_PORTJ_DIR_R   (*((volatile unsigned long *) 0x40060400))
#define GPIO_PORTJ_DEN_R   (*((volatile unsigned long *) 0x4006051C))
#define GPIO_PORTJ_PUR_R   (*((volatile unsigned long *) 0x40060510))
#define GPIO_PORTJ_DATA_RD (*((volatile unsigned long *) 0x400603FC))

// Port pin definitions
#define GPIO_PORTJ_PIN1 0x02
#define GPIO_PORTN_PIN1 0x02

// Default clock frequency and delay definition
#define SYSTEM_CLOCK_FREQUENCY     16000000
#define DELAY_VALUE                SYSTEM_CLOCK_FREQUENCY/1000

//NVIC interrupt enableconfiguration
#define NVIC_EN1_INT51   0x????????   // Interrupt 51 enable

// IRQ 32 to 63 Set Enable Register
#define NVIC_EN1_R       (*((volatile unsigned long *) 0xE000E104))

// IRQ 48 to 51 Priority Register
#define NVIC_PRI12_R     (*((volatile unsigned long *) 0xE000E430))

// DIR      AFSEL   PMC         IS      IBE     IEV     IME     Port mode
// 0        0       0000        0       0       0       1       Input, falling edge trigger, interrupt

// GPIOPort J Interrupt Configuration
#define GPIO_PORTJ_IS_R  (*((volatile unsigned long *) 0x40060404))
#define GPIO_PORTJ_IBE_R (*((volatile unsigned long *) 0x40060408))
#define GPIO_PORTJ_IEV_R (*((volatile unsigned long *) 0x4006040C))
#define GPIO_PORTJ_IM_R  (*((volatile unsigned long *) 0x40060410))
#define GPIO_PORTJ_ICR_R (*((volatile unsigned long *) 0x4006041C))

// Function prototypes
void Delay ( unsigned long counter );
void init_GPIO_N(void);

// user main programint 
main() {

    // so that it doesn't end.
    while(1) {}
}

// Interrupt service routine for Port J
void GPIOPortJ_Handler ( void ){
    GPIO_PORTJ_ICR_R = 0x10;
}

// Function Delay implements the delay
void Delay ( unsigned long counter ){
    unsigned long i = 0; 
    for (i =0; i < counter ; i ++);
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