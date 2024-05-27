//@author: Benett Holmstrom
//@email: benettholmstrom@gmail.com
// This project is an analog-to-digital converter (ADC), which translates an analog signal derived from an
// IR detector and phototransistor into a digital signal usable by a computer. This project was designed to be ran
// on an MKV11Z FPGA with the IR and phototransistor components wired on a separate board.
#include <MKV11Z7.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "gpio_defs.h"
#include "switches.h"
#include "user_defs.h"

#define IR_LED_POS 2

/*
@init_ADC: Enable clock to the analog to digital converter (Port A2)
*/
void init_ADC(void) {
		SIM->SCGC6 |= (1UL<<SIM_SCGC6_ADC0_SHIFT);
		ADC0->CFG1=0x9C;
		ADC0->SC2 = 0;
}

/*
@Measure_IR: Detect the infrared signature of the transistor.
						 The result should be the complement of its initial value,
						 since voltage decreases as infrared signatures increase.
						 So, as infrared signature increases, @res should increase.
@return: res
*/
unsigned Measure_IR(void) {
		volatile unsigned res=0;
		ADC0->SC1[0] = 0x00;
		while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK))
			;
		res = 0xFFFF - (ADC0->R[0]); // The complement of res is the max possible value minus the initial value
		return res;
}
void Delay(unsigned int time_del) {
    // delay is about 1 millisecond * time_del
    volatile int t;
    
    while (time_del--) {
   	 for (t = 4800; t > 0; t--)
   		 ;
    }
}




extern volatile unsigned switch_pressed;
extern void init_debug_signals(void);

int main (void) {
    
		init_ADC();
		init_RGB_LEDs();
	
		SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTA_MASK; // Enable the clock to Port A
	
		PORTA->PCR[IR_LED_POS] &= ~PORT_PCR_MUX_MASK; 
		PORTA->PCR[IR_LED_POS] |= PORT_PCR_MUX(1);
	
		FGPIOA->PDDR |= MASK(IR_LED_POS);
		FGPIOA->PSOR = MASK(IR_LED_POS);
	
		while(1) {
			if (Measure_IR() > 0) {
						FGPIOA->PTOR = MASK(IR_LED_POS); // Enable the IR_LED's bit to port A out
						control_RGB_LEDs(0, 0, 1); // When there is a signal, flash a blue light
			} else {
					FGPIOA->PTOR = 0x0000; // If there is no signal, set A out's bit to zeroes
					control_RGB_LEDs(0, 0, 0); // When there is no signal, turn the light off
			}
			}
    
    }

