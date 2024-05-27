#ifndef LEDS_H
#define LEDS_H

// Freedom KV11z LEDs
#define RED_LED_POS (6)		// on port D
#define GREEN_LED_POS (29)	// on port E
#define BLUE_LED_POS (25)		// on port E

// function prototypes
void init_RGB_LEDs(void);
void control_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on);
void toggle_RGB_LEDs(unsigned int red_on, unsigned int green_on, unsigned int blue_on);

#endif 
