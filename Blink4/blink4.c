//*****************************************************************************
//
// blinky.c - Lab 1
//
//*****************************************************************************

#include <stdint.h>
#include "tm4c1294ncpdt.h"

extern void asm_main(void) ;

//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// Blink the on-board LED(s).
//
//*****************************************************************************

void init_tmc1294 ( )   // Initializes the TM4C1294
{	  	
	  //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R12;

    //
    // Do a dummy read to insert a few cycles after enabling the peripheral.
    //
    volatile uint32_t  portVal = SYSCTL_RCGCGPIO_R;

    //
    // Enable the GPIO pin for the 1st LED (PN0).  
	  // Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIO_PORTN_DIR_R = GPIO_PORTN_DIR_R | 0x01;
    GPIO_PORTN_DEN_R = GPIO_PORTN_DIR_R | 0x01;

    //
    // Enable the GPIO pin for the 2nd LED (PN1).  
	  // Set the direction as output, and
    // enable the GPIO pin for digital function.
    //
    GPIO_PORTN_DIR_R = GPIO_PORTN_DIR_R | 0x02;
    GPIO_PORTN_DEN_R = GPIO_PORTN_DIR_R | 0x02;

}	

void delay()
{
	uint32_t ui32Loop;
	uint32_t ui32Loop2;
  for(ui32Loop = 0; ui32Loop < 16000; ui32Loop++)
  {
		 for(ui32Loop2 = 0; ui32Loop2 < 110; ui32Loop2++)
		{
			
		}
  }
}	

void led1_on_off(uint8_t on)
{
  if (on) 
		// Turn on the 1st LED if on != 0.
		//
    GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R | 0x01;
	
	else 
   // Otherwise, turn off the 1st LED.
   //
   GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R & ~(0x01);		
}	

void led2_on_off(uint8_t on)
{
  if (on) 
		// Turn on the 2nd LED if on != 0.
		//
    GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R | 0x02;
	
	else 
   // Otherwise, turn off the 2nd LED.
   //
   GPIO_PORTN_DATA_R = GPIO_PORTN_DATA_R & ~(0x02);		
}	


int
main(void)
{   
		// uncomment asm_main() to 
	  // run assembly language code in asm_main.s
	  // asm_main () ; 
	
	  // Initialize the microcontroller 
	  init_tmc1294 ();  

    //
    // Loop forever.
    //
    while(1)
    {
        // Turn on the 1st LED.
        led1_on_off(1);
				led2_on_off(0);
        // Delay for a bit.
			  delay();
				led1_on_off(1);
				led2_on_off(1);
			
			  delay();
        // Turn off the 1st LED.
        led1_on_off(0);
				led2_on_off(1);
        // Delay for a bit.
			  delay();
			  led1_on_off(0);
				led2_on_off(0);
        // Delay for a bit.
			  delay();
    }
}
