/**
 *
 * @file user_io_config.h
 * @version 1.0.0
 *
 * ------------------------------
 *
 * @author https://github.com/AJ747
 * @date Jul 12, 2025
 *
 * ------------------------------
 *
 * @brief Definitions for user_io library
 *
 */

#ifndef USER_IO_INC_USER_IO_CONFIG_H_
#define USER_IO_INC_USER_IO_CONFIG_H_



//---------------------------//
// Define begin
//---------------------------//
/// Comment out if alternative method is used
//#define ALTERNATIVE_IRQ_METHOD								// <-- EDIT HERE			
//---------------------------//
// Define end
//---------------------------//



//---------------------------//
// Define begin
//---------------------------//
/// USER IO
// Here the IRQ handler is called every 10 ms
#define USER_IO_HANDLER_PERIOD_MS TIMx_PERIOD_MS				// <-- EDIT HERE

/// BTN
// Amount of buttons used
#define BTNS_AMOUNT	3											// <-- EDIT HERE
// Button sampling time, alter if faster clicking is required
#define BTN_DEBOUNCE_TRESHOLD_MS 20								// <-- EDIT HERE
// Time to wait before button press is registered as hold
#define BTN_HOLD_TRESHOLD_MS 1000							 	// <-- EDIT HERE

/// LED
// Amount of LEDs used
#define LEDS_AMOUNT	3		 									// <-- EDIT HERE
//---------------------------//
// Define end
//---------------------------//



//---------------------------//
// Enum begin
//---------------------------//
// Btn
enum btn_state {
	depressed = 0,
	pressed = 1,
};

enum btn_id {
	BTN0 = 0,  	// <-- EDIT HERE
	BTN1,  		// <-- EDIT HERE
	BTN2,  		// <-- EDIT HERE
	BTN3  		// <-- EDIT HERE
};

// LED
enum led_id {
	LED0 = 0,  	// <-- EDIT HERE
	LED1,  		// <-- EDIT HERE
	LED2  		// <-- EDIT HERE
};
//---------------------------//
// Enum end
//---------------------------//



#endif /* USER_IO_INC_USER_IO_CONFIG_H_ */
