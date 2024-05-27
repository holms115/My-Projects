#include "gpio_defs.h"
#include <MKV11Z7.h>

void init_debug_signals(void) {
	SIM->SCGC5 |=  SIM_SCGC5_PORTB_MASK; /* enable clock for port B */

	/* Select GPIO for pins connected to debug signals*/
	PORTB->PCR[DBG_ISR_POS] |= PORT_PCR_MUX(1);	
	PORTB->PCR[DBG_MAIN_POS] |= PORT_PCR_MUX(1);	
	
	/* Set bits to outputs */
	FGPIOB->PDDR |= MASK(DBG_ISR_POS) | MASK(DBG_MAIN_POS); 
	
	/* Clear output signals initially */
	FGPIOB->PCOR |=MASK(DBG_ISR_POS) | MASK(DBG_MAIN_POS);
}
