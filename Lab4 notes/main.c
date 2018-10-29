# define SYSCTL_RCGC2_R      (*(( volatile unsigned long *)0x400FE108 ))
# define SYSCTL_RCGC2_GPIOF  0x0020 // Port F clock gating control
# define GPIO_PORTF_DATA_RD  (*(( volatile unsigned long *)0x40025040 ))
# define GPIO_PORTF_DATA_WR  (*(( volatile unsigned long *)0x40025020 ))
# define GPIO_PORTF_DIR_R    (*(( volatile unsigned long *)0x40025400 ))
# define GPIO_PORTF_DEN_R    (*(( volatile unsigned long *)0x4002551C ))
# define GPIO_PORTF_PUR_R    (*(( volatile unsigned long *)0x40025510 ))

# define  GPIO_PORTF_PIN3  0x08
# define  GPIO_PORTF_PIN4  0x10

// Default clock frequency
# define  SYSTEM_CLOCK_FREQUENCY     16000000
# define  DEALY_DEBOUNCE             SYSTEM_CLOCK_FREQUENCY /1000

// Function prototypes
void Delay ( unsigned long counter );

// User main program
int main (){
    static char flag = 0;

    // Enable the clock for port F
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF ;

    // Configure PF3 as digital output and PF4 as digital input
    GPIO_PORTF_DEN_R |= GPIO_PORTF_PIN3 + GPIO_PORTF_PIN4 ;
    GPIO_PORTF_DIR_R |= GPIO_PORTF_PIN3 ;

    // PF3 as output
    GPIO_PORTF_DIR_R &= (~ GPIO_PORTF_PIN4 );

    // PF4 as input
    GPIO_PORTF_PUR_R |= 0x10;

    // Enable pull -up on PF4
    while (1) {
        if( GPIO_PORTF_DATA_RD == 0)
        {
            Delay ( DEALY_DEBOUNCE );
            if (( flag == 0) && ( GPIO_PORTF_DATA_RD == 0))
            {
                // Toggle the green LED
                GPIO_PORTF_DATA_WR ^= GPIO_PORTF_PIN3 ;
                flag = 1;
            }
        }
        else // clear the flag only when the switch is released
        {
            flag = 0;
        }
    }
}

/* This function implements the delay */
void Delay ( unsigned long counter ){
    unsigned long i = 0;
    for (i =0; i< counter ; i ++) ;
}
