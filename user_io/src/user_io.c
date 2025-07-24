/**
 *
 * @file user_io.c
 * @version 1.1.0
 *
 * ------------------------------
 *
 * @author https://github.com/AJ747
 * @date Jul 12, 2025
 *
 * ------------------------------
 *
 * @brief Library for buttons and LEDs
 *
 */



//---------------------------//
// Include begin
//---------------------------//
#include "user_io.h"
#include "user_io_driver.h"
//---------------------------//
// Include end
//---------------------------//



//---------------------------//
// Define begin
//---------------------------//
#ifdef BTNS_USE
#define BTNS_IDLE_MS_MAX (0xFFFFFFFF - USER_IO_HANDLER_PERIOD_MS)
#define BTN_DEBOUNCE_TRESHOLD (BTN_DEBOUNCE_TRESHOLD_MS / USER_IO_HANDLER_PERIOD_MS)
#endif
//---------------------------//
// Define end
//---------------------------//



//---------------------------//
// Enum begin
//---------------------------//
#ifdef LEDS_USE
enum led_state {
	OFF = 0,
	ON,
	BLINK_INFINITE,
	BLINK_MS,
	BLINK_N_TIMES,
	PULSE
};
#endif
//---------------------------//
// Enum end
//---------------------------//



//---------------------------//
// Struct begin
//---------------------------//
#ifdef BTNS_USE
struct btn {
	enum btn_id id;
	enum btn_state curr_state;
	enum btn_state last_state;
	uint8_t debounce_counter;
	uint8_t press_counter;
	uint8_t click;
	uint8_t released;
	uint16_t hold_duration;
};
#endif



#ifdef LEDS_USE
struct led {
	enum led_id id;
	enum led_state set_state;
	enum led_state curr_state;
	int16_t blink_counter;
	uint16_t blink_rate;
	int16_t blink_duration;
	int16_t pulse_duration;
};
#endif
//---------------------------//
// Struct end
//---------------------------//



//---------------------------//
// Prototypes begin
//---------------------------//
#ifdef BTNS_USE
static void btns_init(void);
static void btns_handle_states(void);
static void btn_debounce(enum btn_id id);
#endif



#ifdef LEDS_USE
static void leds_init(void);
static void leds_handle_effects(void);
static void led_handle_effect_blink_infinite(enum led_id id);
static void led_handle_effect_blink_ms(enum led_id id);
static void led_handle_effect_blink_n_times(enum led_id id);
static void led_handle_effect_off(enum led_id id);
static void led_handle_effect_pulse(enum led_id id);
static void led_handle_effect_on(enum led_id id);
#endif



#ifdef INTERVALS_USE
static void intervals_init(void);
static void intervals_update(void);
#endif
//---------------------------//
// Prototypes end
//---------------------------//



//---------------------------//
// Variable begin
//---------------------------//
#ifdef ALTERNATIVE_IRQ_METHOD
volatile uint8_t user_io_handle_rdy = 0;
#endif



#ifdef BTNS_USE
static uint32_t btns_idle_counter_ms = 0;
static struct btn btn[BTNS_AMOUNT];
#endif



#ifdef LEDS_USE
static struct led led[LEDS_AMOUNT];
#endif



#ifdef INTERVALS_USE
static uint32_t interval[INTERVALS_AMOUNT] = {0};
#endif
//---------------------------//
// Variable end
//---------------------------//



/**
 * @fn void user_io_init(void)
 * @brief Inits used mcu-periphs and framework used
 * 
 */
void user_io_init(void) {
#ifdef SWITCHES_USE
	switch_pins_init();
#endif



#ifdef BTNS_USE
	btn_pins_init();
	btns_init();
#endif
	


#ifdef LEDS_USE
	led_pins_init();
	leds_init();
#endif


	
#ifdef INTERVALS_USE
	intervals_init();
#endif
}



/**
 * @fn void user_io_irq_handler(void)
 * @brief Checks and updates states
 * 
 * @note Must be called with fixed interval 
 */
void user_io_irq_handler(void) {
#ifdef BTNS_USE
	btns_handle_states();
#endif



#ifdef LEDS_USE
	leds_handle_effects();
#endif


	
#ifdef INTERVALS_USE
	intervals_update();
#endif
}



#ifdef SWITCHES_USE
/**
 * @fn uint8_t switch_check(enum switch_id)
 * @brief Checks if switch is on
 * 
 * @param id (uint8_t) swtich id
 * @return (uint8_t) true or false
 */
uint8_t switch_on(enum switch_id id) {
	return switch_get_state(id);
}



/**
 * @fn uint8_t switch_off(enum switch_id)
 * @brief Checks if switch is off
 * 
 * @param id (uint8_t) switch id
 * @return (uint8_t) true or false
 */
uint8_t switch_off(enum switch_id id) {
	return !switch_get_state(id);
}
#endif



#ifdef BTNS_USE
/**
 * @fn uint8_t btn_hold(enum btn_id)
 * @brief Checks if button is held down more than threshold in milliseconds
 * 
 * @param id (enum btn_id) button to check
 * @return (uint8_t) true or false
 * 
 * @note Always check for the longest hold duration first to avoid missing longer hold events.
 */
uint8_t btn_hold_ms(enum btn_id id, uint16_t ms) {
	return (btn[id].hold_duration >= ms)? true : false;
}



/**
 * @fn uint8_t btn_depressed(enum btn_id)
 * @brief Checks if button is depressed
 * 
 * @param id (enum btn_id) button to check
 * @return (uint8_t) true (1) or false (0)
 */
uint8_t btn_depressed(enum btn_id id) {
	return !btn[id].hold_duration;
}



/**
 * @fn uint8_t btn_released(enum btn_id)
 * @brief Checks if button is released
 * 
 * @param id (enum btn_id) button to check
 * @return (uint8_t) true (1) or false (0)
 */
uint8_t btn_released(enum btn_id id) {
	uint8_t temp = btn[id].released;
	
	// Reset state
	btn[id].released = false;
	
	return temp;
}



/**
 * @fn uint8_t btn_click(enum btn_id)
 * @brief Checks if click is registered on specified button
 * 
 * @param id (enum btn_id) button to check
 * @return (uint8_t) true (1) or false (0)
 */
uint8_t btn_click(enum btn_id id) {
	uint8_t temp = btn[id].click;
	
	// Reset state
	btn[id].click = false;
	
	return temp;
}



/**
 * @fn uint8_t btns_no_input_ms(uint32_t)
 * @brief Checks for lack of input on all btns for more than idle_ms
 * 
 * @param idle_ms (uint32_t) no input threshold
 * @return (uint8_t) true or false
 */
uint8_t btns_no_input_ms(uint32_t idle_ms) {
	if (btns_idle_counter_ms > idle_ms) {
		return true;
	}
	return false;
}



/**
 * @fn void btns_init(void)
 * @brief Inits all buttons with default params
 * 
 */
static void btns_init(void) {
	for (uint8_t id = 0; id < BTNS_AMOUNT; id++) {
		btn[id].id = id;
		btn[id].curr_state = BTN_DEPRESSED;
		btn[id].last_state = BTN_DEPRESSED;
		btn[id].debounce_counter = 0;
		btn[id].press_counter = 0;
		btn[id].click = false;		
		btn[id].hold_duration = 0;
		btn[id].released = false;
	}
}



/**
 * @fn void btns_handle_states(void)
 * @brief Update btn states, check for click or hold etc
 * 
 */
static void btns_handle_states(void) {
	if (btns_idle_counter_ms < BTNS_IDLE_MS_MAX) {
		btns_idle_counter_ms += USER_IO_HANDLER_PERIOD_MS;
	}
	
	for (uint8_t id = 0; id < BTNS_AMOUNT; id++) {
		btn_debounce(id);
		
		// Check for click
		if ((btn[id].curr_state == BTN_PRESSED) && (btn[id].last_state == BTN_DEPRESSED)) {
			btn[id].click = true;
			btns_idle_counter_ms = 0;
			
		// Check for hold 
		} else if ((btn[id].curr_state == BTN_PRESSED) && (btn[id].last_state == BTN_PRESSED)) {
			btn[id].hold_duration += USER_IO_HANDLER_PERIOD_MS;		
			btns_idle_counter_ms = 0;
			
		// Check for release
		} else if ((btn[id].curr_state == BTN_DEPRESSED) && (btn[id].last_state == BTN_PRESSED)) {
			btn[id].released = true;
			btn[id].hold_duration = 0;
		}
		

		btn[id].last_state = btn[id].curr_state;
	}
}



/**
 * @fn void btn_debounce(enum btn_id)
 * @brief Debounces specific button
 * 
 * @param id (enum btn_id) button to debounce
 */
static void btn_debounce(enum btn_id id) {
	btn[id].press_counter += (uint8_t) btn_get_state(id);
			
	// Debounce btn
	if (btn[id].debounce_counter >= BTN_DEBOUNCE_TRESHOLD) {
		
		// Register as press
		if (btn[id].press_counter > 0) {
			btn[id].curr_state = BTN_PRESSED;
			btn[id].press_counter = 0;
			
		// Register as depressed, reset state
		} else {
			btn[id].curr_state = BTN_DEPRESSED;
		}
		
		btn[id].debounce_counter = 0;
	} else {
		btn[id].debounce_counter++;
	}
}
#endif



#ifdef LEDS_USE
/**
 * @fn void led_blink_infinite(enum led_id, uint16_t)
 * @brief Applies infinite blinking effect to specified LED
 * 
 * @param id (enum led_id) LED to apply effect to
 * @param blink_rate_ms (uint16_t) time in ms LED is on and off
 */
void led_blink_infinite(enum led_id id, uint16_t blink_rate_ms) {
	led[id].set_state = BLINK_INFINITE;
	led[id].blink_rate = blink_rate_ms;
}



/**
 * @fn void led_blink_ms(enum led_id, uint16_t, uint16_t)
 * @brief Applies finite blinking effect to specified LED
 * 
 * @param id (enum led_id) LED to apply effect to
 * @param blink_rate_ms (uint16_t) time in ms LED is on and off
 * @param duration_ms (uint16_t) blinking effect time ms
 */
void led_blink_ms(enum led_id id, uint16_t blink_rate_ms, uint16_t duration_ms) {
	led[id].set_state = BLINK_MS;
	led[id].blink_rate = blink_rate_ms;
	led[id].blink_duration = (int16_t) duration_ms;
}



/**
 * @fn void led_blink_n_times(enum led_id, uint16_t, uint16_t)
 * @brief Applies finite blinking effect to specified LED
 * 
 * @param id (enum led_id) LED to apply effect to
 * @param blink_rate_ms (uint16_t) time in ms LED is on and off
 * @param n (uint16_t) how many times LED blinks
 */
void led_blink_n_times(enum led_id id, uint16_t blink_rate_ms, uint16_t n) {
	led[id].set_state = BLINK_N_TIMES;
	led[id].blink_rate = blink_rate_ms;
	led[id].blink_duration = (int16_t) (n<<1); // Double, because off and on counts as 1 time each 
}



/**
 * @fn void led_off(enum led_id)
 * @brief Turns off specified LED
 * 
 * @param id (enum led_id) LED to turn off
 * 
 * @note Only applies to LEDs that are on or blinking infinitely,
 * does not apply to finite effects. See led_force_off() for an alternative
 */
void led_off(enum led_id id) {
	// Let effects with duration finish
	if ((led[id].set_state == ON) || (led[id].set_state == BLINK_INFINITE)) {
		led[id].set_state = OFF;
		
		// Makes sure start of blink effect is the same
		led[id].blink_counter = 0;
	}
}



/**
 * @fn void led_force_off(enum led_id)
 * @brief Forces off specified LED no matter what effect is ongoing
 * 
 * @param id (enum led_id) LED to turn off
 */
void led_force_off(enum led_id id) {
	led[id].set_state = OFF;
	
	// Makes sure start of blink effect is the same
	led[id].blink_counter = 0;
}



/**
 * @fn void led_on(enum led_id)
 * @brief Turns on specified LED
 * 
 * @param id (enum led_id) LED to turn on
 */
void led_on(enum led_id id) {
	led[id].set_state = ON;
}



/**
 * @fn void led_pulse(enum led_id, uint16_t)
 * @brief Pulse LED
 * 
 * @param id (enum led_id) LED to pulse
 * @param pulse_duration_ms (uint16_t) how long LED is on ms
 */
void led_pulse(enum led_id id, uint16_t pulse_duration_ms) {
	led[id].set_state = PULSE;
	led[id].pulse_duration = (int16_t) pulse_duration_ms;
}



/**
 * @fn void led_all_off(void)
 * @brief Turn all LEDs off
 * 
 * @note Only applies to LEDs that are on or blinking infinitely,
 * does not apply to finite effects. See led_all_force_off() for an alternative
 */
void led_all_off(void) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		led_off(id);
	}
}



/**
 * @fn void led_all_force_off(void)
 * @brief Forces all LEDs off no matter what effect is ongoing
 * 
 */
void led_all_force_off(void) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		led_force_off(id);
	}
}



/**
 * @fn void led_all_on(void)
 * @brief Turn all LEDs on
 * 
 */
void led_all_on(void) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		led_on(id);
	}
}



/**
 * @fn void led_all_blink_infinite(uint16_t)
 * @brief Apply infinite blinking effect to all LEDs
 * 
 * @param blink_rate_ms (uint16_t) time in ms LEDs are on and off
 */
void led_all_blink_infinite(uint16_t blink_rate_ms) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		led_blink_infinite(id, blink_rate_ms);
	}
}



/**
 * @fn void led_all_blink_n_times(uint16_t, uint16_t)
 * @brief Apply finite blinking effect to all LEDs
 * 
 * @param blink_rate_ms (uint16_t) time in ms LEDs are on and off
 * @param n (uint16_t) how many times LEDs blink
 */
void led_all_blink_n_times(uint16_t blink_rate_ms, uint16_t n) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		led_blink_n_times(id, blink_rate_ms, n);
	}
}



/**
 * @fn void led_all_blink_ms(uint16_t, uint16_t)
 * @brief Apply finite blinking effect to all LEDs
 * 
 * @param blink_rate_ms (uint16_t) time in ms LEDs are on and off
 * @param duration_ms (uint16_t) blinking effect time ms
 */
void led_all_blink_ms(uint16_t blink_rate_ms, uint16_t duration_ms) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		led_blink_ms(id, blink_rate_ms, duration_ms);
	}
}



/**
 * @fn void led_all_pulse(uint16_t)
 * @brief Pulse all LEDs
 * 
 * @param pulse_duration_ms (uint16_t) how long LEDs are on ms
 */
void led_all_pulse(uint16_t pulse_duration_ms) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		led_pulse(id, pulse_duration_ms);
	}
}



/**
 * @fn void leds_init(void)
 * @brief Inits all LEDs with default params
 * 
 */
static void leds_init(void) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		led[id].id = id;
		led[id].set_state = OFF;
		led[id].curr_state = OFF;
		led[id].blink_counter = 0;
		led[id].blink_rate = 0;	
		led[id].blink_duration = 0;
		led[id].pulse_duration = 0;
	}
}



/**
 * @fn void leds_handle_effects(void)
 * @brief Apply effects and animate them for all LEDs
 * 
 */
static void leds_handle_effects(void) {
	for (uint8_t id = 0; id < LEDS_AMOUNT; id++) {
		switch (led[id].set_state) {
		
			case BLINK_INFINITE:
				led_handle_effect_blink_infinite(id);
				break;				
				
			case BLINK_MS:
				led_handle_effect_blink_ms(id);
				break;
						
			case BLINK_N_TIMES:
				led_handle_effect_blink_n_times(id);
				break;
												
			case OFF:
				led_handle_effect_off(id);
				break;
											
			case PULSE:
				led_handle_effect_pulse(id);
				break;
				
			case ON:
				led_handle_effect_on(id);
				break;
		}
	}
}



/**
 * @fn void led_effect_blink_infinite(enum led_id)
 * @brief Handles blink infinite effect
 * 
 * @param id (enum led_id) LED
 */
static void led_handle_effect_blink_infinite(enum led_id id) {
	// Time to toggle
	if (led[id].blink_counter <= 0) {
		led_driver_toggle(id);
		led[id].curr_state = BLINK_INFINITE;
		
		// Reset counter
		led[id].blink_counter = (int16_t) led[id].blink_rate;
	}
	
	// Remaining time to toggle
	led[id].blink_counter -= USER_IO_HANDLER_PERIOD_MS;
}



/**
 * @fn void led_handle_effect_blink_ms(enum led_id)
 * @brief Handles blink ms effect
 * 
 * @param id (enum led_id) LED
 */
static void led_handle_effect_blink_ms(enum led_id id) {
	// Effect is done
	if (led[id].blink_duration <= 0) {
		led_driver_off(id);
		
		led[id].set_state = OFF;
		led[id].curr_state = OFF;
		return;
	}
	
	// Calc remaining effect time
	led[id].blink_duration -= USER_IO_HANDLER_PERIOD_MS;
	
	// Time to toggle
	if (led[id].blink_counter <= 0) {
		led_driver_toggle(id);
		led[id].curr_state = BLINK_MS;
		
		// Reset counter
		led[id].blink_counter = (int16_t) led[id].blink_rate;
	}
	
	// Remaining time to toggle
	led[id].blink_counter -= USER_IO_HANDLER_PERIOD_MS;
}



/**
 * @fn void led_handle_effect_blink_n_times(enum led_id)
 * @brief Handles blink n times effect
 * 
 * @param id (enum led_id) LED
 */
static void led_handle_effect_blink_n_times(enum led_id id) {
	// N times reached
	if (led[id].blink_duration <= 0) {
		led_driver_off(id);
		
		led[id].set_state = OFF;
		led[id].curr_state = OFF;
		return;
	}
	
	// Time to toggle
	if (led[id].blink_counter <= 0) {
		led_driver_toggle(id);
		led[id].curr_state = BLINK_N_TIMES;
		
		// Reset counter
		led[id].blink_counter = (int16_t) led[id].blink_rate;

		// Update total blinks left
		led[id].blink_duration--;
	}
	
	// Remaining time to toggle
	led[id].blink_counter -= USER_IO_HANDLER_PERIOD_MS;
}



/**
 * @fn void led_handle_effect_off(enum led_id)
 * @brief Handles off effect
 * 
 * @param id (enum led_id) LED
 */
static void led_handle_effect_off(enum led_id id) {
	if (led[id].curr_state != OFF) {
		led_driver_off(id);	
		led[id].curr_state = OFF;
	} 
}



/**
 * @fn void led_handle_effect_pulse(enum led_id)
 * @brief Handles pulse effect
 * 
 * @param id (enum led_id) LED
 */
static void led_handle_effect_pulse(enum led_id id) {
	// Pulse effect done
	if (led[id].pulse_duration <= 0) {
		led_driver_off(id);
		
		led[id].set_state = OFF;
		led[id].curr_state = OFF;
		return;
		
	// Start pulse effect
	} else if (led[id].curr_state != PULSE) {
		led_driver_on(id);
		
		led[id].curr_state = PULSE;
	}
		
	// Remaining effect time
	led[id].pulse_duration -= USER_IO_HANDLER_PERIOD_MS;	
}



/**
 * @fn void led_handle_effect_on(enum led_id)
 * @brief Handles on effect
 * 
 * @param id (enum led_id) LED
 */
static void led_handle_effect_on(enum led_id id) {
	if (led[id].curr_state != ON) {
		led_driver_on(id);
		led[id].curr_state = ON;
	}
}
#endif



#ifdef INTERVALS_USE
/**
 * @fn uint8_t interval_reached_ms(enum interval_id, uint32_t)
 * @brief Checks to see if ms interval is reached since last check
 * 
 * @param id (enum interval_id) interval to check
 * @param ms (uint32_t) run at ms interval
 * @return (uint8_t) true or false
 * 
 * @note Can count up to ~49 days, 2^32 ms, before overflow
 */
uint8_t interval_reached_ms(enum interval_id id, uint32_t ms) {
	if (interval[id] >= ms) {
		interval[id] = 0;
		
		return true;
	}
	return false;
}



/**
 * @fn void intervals_init(void)
 * @brief Inits interval counters
 */
static void intervals_init(void) {
	for (uint8_t id = 0; id < INTERVALS_AMOUNT; id++) {
		interval[id] = 0;
	}
}



/**
 * @fn void intervals_update(void)
 * @brief Updates counters for all intervals
 */
static void intervals_update(void) {
	for (uint8_t id = 0; id < INTERVALS_AMOUNT; id++) {
		interval[id] += USER_IO_HANDLER_PERIOD_MS;
	}
}
#endif