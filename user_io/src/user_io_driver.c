/**
 *
 * @file user_io_driver.c
 * @version 1.1.0
 *
 * ------------------------------
 *
 * @author https://github.com/AJ747
 * @date Jul 12, 2025
 *
 * ------------------------------
 *
 * @brief Driver-layer source
 *
 */



//---------------------------//
// Include begin
//---------------------------//
#include "user_io_driver.h"
//---------------------------//
// Include end
//---------------------------//



#ifdef SWITCHES_USE
/**
 * @fn void btn_pins_init(void)
 * @brief Inits all switch-pins and applies internal pull-up // <-- EDIT HERE
 */
void switch_pins_init(void) {
	PIN_CONFIG(SW0_PIN, INPUT_PULL_UP); 	// <-- EDIT HERE
	PIN_CONFIG(SW1_PIN, INPUT_PULL_UP); 	// <-- EDIT HERE
}



/**
 * @fn enum switch_state switch_get_state(enum switch_id)
 * @brief Returns state of switch
 * 
 * @param id (enum switch_id) switch id
 * @return (enum switch_state) SWITCH_OFF or SWITCH_ON
 */
enum switch_state switch_get_state(enum switch_id id) {
	switch (id) {
		case SW0:
			return !PIN_READ(SW0_PIN); 	// <-- EDIT HERE
			
		case SW1:
			return !PIN_READ(SW1_PIN); 	// <-- EDIT HERE
		
		default:
			return SWITCH_OFF;
	}
}
#endif



#ifdef BTNS_USE
/**
 * @fn void btn_pins_init(void)
 * @brief Inits all button-pins and applies internal pull-up
 * 
 * @note Internal pull-up used, active low 	// <-- EDIT HERE
 */
void btn_pins_init(void) {
	PIN_CONFIG(BTN0_PIN, INPUT_PULL_UP); 	// <-- EDIT HERE
	PIN_CONFIG(BTN1_PIN, INPUT_PULL_UP); 	// <-- EDIT HERE
	PIN_CONFIG(BTN2_PIN, INPUT_PULL_UP); 	// <-- EDIT HERE
}



/**
 * @fn enum btn_state btn_get_state(enum btn_id)
 * @brief Returns state of specific button
 * 
 * @param id (enum btn_id) button to read from
 * @return (enum btn_state) depressed (0) or pressed (1)
 */
enum btn_state btn_get_state(enum btn_id id) {
	switch (id) {
		case BTN0: 
			return !PIN_READ(BTN0_PIN); 	// <-- EDIT HERE

		case BTN1: 
			return !PIN_READ(BTN1_PIN); 	// <-- EDIT HERE
			
		case BTN2: 
			return !PIN_READ(BTN2_PIN); 	// <-- EDIT HERE
			
		default:
			return BTN_DEPRESSED;
	}
}
#endif



#ifdef LEDS_USE
/**
 * @fn void led_pins_init(void)
 * @brief Inits all LED-pins and turns them off
 * 
 */
void led_pins_init(void) {
	PIN_CONFIG(LED0_PIN, OUTPUT); 	// <-- EDIT HERE
	PIN_CONFIG(LED1_PIN, OUTPUT); 	// <-- EDIT HERE
	PIN_CONFIG(LED2_PIN, OUTPUT); 	// <-- EDIT HERE

	PIN_LOW(LED0_PIN);				// <-- EDIT HERE
	PIN_LOW(LED1_PIN);				// <-- EDIT HERE
	PIN_LOW(LED2_PIN);				// <-- EDIT HERE
}



/**
 * @fn void led_driver_on(enum led_id)
 * @brief Turns on specific LED
 * 
 * @param id (enum led_id) 
 */
void led_driver_on(enum led_id id) {
	switch (id) {
		case LED0:
			PIN_HIGH(LED0_PIN); 	// <-- EDIT HERE
			break;

		case LED1:
			PIN_HIGH(LED1_PIN); 	// <-- EDIT HERE
			break;

		case LED2:
			PIN_HIGH(LED2_PIN); 	// <-- EDIT HERE
			break;
	}
}



/**
 * @fn void led_driver_off(enum led_id)
 * @brief Turns off specific LED
 * 
 * @param id (enum led_id) LED to turn off
 */
void led_driver_off(enum led_id id) {
	switch (id) {
		case LED0:
			PIN_LOW(LED0_PIN);		// <-- EDIT HERE
			break;
			
		case LED1:
			PIN_LOW(LED1_PIN);		// <-- EDIT HERE
			break;

		case LED2:
			PIN_LOW(LED2_PIN);		// <-- EDIT HERE
			break;
	}
}



/**
 * @fn void led_driver_toggle(enum led_id)
 * @brief Toggles specific LED
 * 
 * @param id (enum led_id) LED to toggle 
 */
void led_driver_toggle(enum led_id id) {
	switch (id) {
		case LED0:
			PIN_TOGGLE(LED0_PIN); 	// <-- EDIT HERE
			break;
			
		case LED1:
			PIN_TOGGLE(LED1_PIN); 	// <-- EDIT HERE
			break;

		case LED2:
			PIN_TOGGLE(LED2_PIN); 	// <-- EDIT HERE
			break;
	}
}
#endif