//toggling D3 (PF4) on board
#define SYSCTL_RCGC2_R (*((volatile unsigned long *)0x400FE608 ))
#define SYSCTL_RCGC2_GPIOF 0x00000020 // Port F clock gating control
#define GPIO_PORTF_AHB_DATA_R (*(( volatile unsigned long *)0x4005D3FC))
#define GPIO_PORTF_AHB_DIR_R (*(( volatile unsigned long *)0x4005D400))
#define GPIO_PORTF_AHB_DEN_R (*(( volatile unsigned long *)0x4005D51C))
#define GPIO_PORTF_AHB_AFSEL_R (*(( volatile unsigned long *)0x4005D420))
#define SYSTEM_CLOCK_FREQUENCY 16000000 // Default clock frequency
#define DELAY_VALUE SYSTEM_CLOCK_FREQUENCY/10
#define GPIO_PORTF_PIN4 0x10 // PN0, LED D3 on board
#define GPIO_PORTF_AHB_AFSEL_DIS 0x00 //disable alternate function
void delay(unsigned long j);
// User main program
int main (){
	// Andrei From The Great White North tips
	// 1) turn on the clock for the port
	// 2) configure the direction of the bits in the port
	// 3) give the bit an initial value
	// 4) turn on the port
	
	// Enable the clock for port F
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;
	// enable port F pin 4 as digital output
	GPIO_PORTF_AHB_DEN_R |= GPIO_PORTF_PIN4;
	// Digital enable for PF4
	GPIO_PORTF_AHB_DIR_R |= GPIO_PORTF_PIN4;
	// disable the alternate functionality
	GPIO_PORTF_AHB_AFSEL_R &= GPIO_PORTF_AHB_AFSEL_DIS;
	// PF4 as output
	while (1) {
		// Toggle the LED
		GPIO_PORTF_AHB_DATA_R ^= GPIO_PORTF_PIN4;
		unsigned long j;
		delay(j);
	}
}

void delay(unsigned long j) {
	// delay loop
	j = 0;
	while (j < DELAY_VALUE) {
		j++;
	}
}
