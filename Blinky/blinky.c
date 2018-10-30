//toggling D3 (PF4) on board
//	Address		7		6		5		5		4		3		2		1
//	$400F.E108	--		--		GPIOF	GPIOE	GPIOD	GPIOC	GPIOB	GPIOA
# define SYSCTL_RCGC2_R (*((volatile unsigned long *)0x400FE608 )) 
//	
# define SYSCTL_RCGC2_GPIOF 0x00000020 // Port F clock gating control
//	$400F.E108	--		--		DATA	DATA	DATA	DATA	DATA	DATA
# define GPIO_PORTF_AHB_DATA_R (*(( volatile unsigned long *)0x4005D3FC))
# define GPIO_PORTF_AHB_DIR_R (*(( volatile unsigned long *)0x4005D400))
# define GPIO_PORTF_AHB_DEN_R (*(( volatile unsigned long *)0x4005D51C))
# define GPIO_PORTF_AHB_AFSEL_R (*(( volatile unsigned long *)0x4005D420))
# define SYSTEM_CLOCK_FREQUENCY 16000000 // Default clock frequency
# define DELAY_VALUE SYSTEM_CLOCK_FREQUENCY/10
# define GPIO_PORTF_PIN4 0x10 // PF4, LED D3 on board
# define GPIO_PORTF_AHB_AFSEL_DIS 0x00 //disable alternate function

unsigned long j =0;// User main program

int main (){ 
	
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
	
		// delay loop 
		for (j =0; j< DELAY_VALUE ; j ++);
	} 
}
