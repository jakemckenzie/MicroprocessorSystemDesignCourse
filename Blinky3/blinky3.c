#include <stdint.h>
#define SYSCTL_RCGCGPIO_R12     0x00001000  // GPIO Port N Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608))
#define GPIO_PORTN_DATA_R       (*((volatile uint32_t *)0x400643FC))
#define GPIO_PORTN_DIR_R        (*((volatile uint32_t *)0x40064400))
#define GPIO_PORTN_DEN_R        (*((volatile uint32_t *)0x4006451C))
#define GPIO_PORTN_PUR_R        (*((volatile uint32_t *)0x40064510))
// Default clock frequency
#define SYSTEM_CLOCK_FREQUENCY  16000000 // Default clock frequency
#define DELAY_DEBOUNCE          SYSTEM_CLOCK_FREQUENCY / 1000
#define  GPIO_PORTN_PIN1       0x02
#define  GPIO_PORTN_PIN2       0x03

// Function prototypes
void Delay( unsigned long counter );

// main program
int main() {
    volatile char flag = 0;

    // enable the clock for port N
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R12;

    // configure port N 1 as digital output and port N 2 as digital input
    GPIO_PORTN_DEN_R |= GPIO_PORTN_PIN1 + GPIO_PORTN_PIN2;
    GPIO_PORTN_DIR_R |= GPIO_PORTN_PIN1;

    // port N 1 as ouput
    GPIO_PORTN_DIR_R &= (~ GPIO_PORTN_PIN2);

    //Enable pull-up resistor on port N 2
    while(1) {
        if (GPIO_PORTN_DATA_R == 0) {
            Delay(DELAY_DEBOUNCE);
            if ((flag == 0) && (GPIO_PORTN_DATA_R)) {  // toggle the LED
                GPIO_PORTN_DATA_R ^= GPIO_PORTN_PIN1;
                flag = 1;
            } else {    // clear the flag only when the switch is released
                flag = 0;
            }
        }
    }
}

/**
 * this funtion implements a delay
 */

void Delay (unsigned long counter) {
    unsigned long i = 0;
    while (i != counter) {
        i++;
    }
}
