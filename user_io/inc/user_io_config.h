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
/// Uncomment if alternative method is used
//#define ALTERNATIVE_IRQ_METHOD // <-- EDIT HERE



// Comment if feature is not needed
#define BTNS_USE // <-- EDIT HERE
#define LEDS_USE // <-- EDIT HERE
#define INTERVALS_USE // <-- EDIT HERE



// IRQ handler is called every 10 ms // <-- EDIT HERE
#define USER_IO_HANDLER_PERIOD_MS TIMx_PERIOD_MS // <-- EDIT HERE



#ifdef BTNS_USE
// Button sampling time, alter if faster clicking is required
#define BTN_DEBOUNCE_TRESHOLD_MS 20	// <-- EDIT HERE
#define BTNS_AMOUNT	3 // <-- EDIT HERE
#endif



#ifdef LEDS_USE
#define LEDS_AMOUNT	3	// <-- EDIT HERE
#endif



#ifdef INTERVALS_USE
#define INTERVALS_AMOUNT 3	// <-- EDIT HERE
#endif
//---------------------------//
// Define end
//---------------------------//



//---------------------------//
// Enum begin
//---------------------------//
#ifdef BTNS_USE
enum btn_state {
	depressed = 0,
	pressed = 1,
};

enum btn_id {
	BTN0 = 0, // <-- EDIT HERE
	BTN1, // <-- EDIT HERE
	BTN2 // <-- EDIT HERE
};
#endif



#ifdef LEDS_USE
enum led_id {
	LED0 = 0, // <-- EDIT HERE
	LED1, // <-- EDIT HERE
	LED2 // <-- EDIT HERE
};
#endif



#ifdef INTERVALS_USE
enum interval_id {
	INTERVAL0 = 0, // <-- EDIT HERE
	INTERVAL1, // <-- EDIT HERE
	INTERVAL2, // <-- EDIT HERE
};
#endif
//---------------------------//
// Enum end
//---------------------------//



#endif /* USER_IO_INC_USER_IO_CONFIG_H_ */
