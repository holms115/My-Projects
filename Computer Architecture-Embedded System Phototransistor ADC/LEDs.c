#include <MKV11Z7.H>
#include "LEDs.h"
#include "gpio_defs.h"

void init_RGB_LEDs(void) {
	// Enable clock to ports e and d
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;;
	
	// Make 3 pins GPIO
	PORTD->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTD->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);          
	PORTE->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);          
	PORTE->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;          
	PORTE->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);          
	
	// Set ports to outputs
	FGPIOD->PDDR |= MASK(RED_LED_POS) ;

	FGPIOE->PDDR |= MASK(BLUE_LED_POS) 	| MASK(GREEN_LED_POS);
	
	//control_RGB_LEDs(1, 1, 1);
	//control_RGB_LEDs(0, 0, 0);
}

void control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on) {
	if (red_on) {
			FGPIOD->PCOR = MASK(RED_LED_POS);
	} else {
			FGPIOD->PSOR = MASK(RED_LED_POS); 
	}
	if (green_on) {
			FGPIOE->PCOR = MASK(GREEN_LED_POS);
	}	else {
			FGPIOE->PSOR = MASK(GREEN_LED_POS); 
	} 
	if (blue_on) {
			FGPIOE->PCOR = MASK(BLUE_LED_POS);
	}	else {
			FGPIOE->PSOR = MASK(BLUE_LED_POS); 
	}
}	

void toggle_RGB_LEDs(unsigned int red_tog, unsigned int green_tog, unsigned int blue_tog) {
	if (red_tog) {
			FGPIOD->PTOR = MASK(RED_LED_POS);
	} 
	if (green_tog) {
			FGPIOE->PTOR = MASK(GREEN_LED_POS);
	} 
	if (blue_tog) {
			FGPIOE->PTOR = MASK(BLUE_LED_POS);
	}	
}	
