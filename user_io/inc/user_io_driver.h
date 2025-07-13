/**
 *
 * @file user_io_driver.h
 * @version 1.0.0
 *
 * ------------------------------
 *
 * @author https://github.com/AJ747
 * @date Jul 12, 2025
 *
 * ------------------------------
 *
 * @brief Driver-layer for buttons and LEDs
 *
 */

#ifndef USER_IO_INC_USER_IO_DRIVER_H_
#define USER_IO_INC_USER_IO_DRIVER_H_



//---------------------------//
// Include begin
//---------------------------//
#include "stdint.h"
#include "stdbool.h"
#include "user_io_config.h"
//---------------------------//
// Include end
//---------------------------//



//---------------------------//
// Prototypes begin
//---------------------------//
#ifdef BTNS_USE
void btn_pins_init(void);
enum btn_state btn_get_state(enum btn_id id);
#endif



#ifdef LEDS_USE
void led_pins_init(void);
void led_driver_on(enum led_id id);
void led_driver_off(enum led_id id);
void led_driver_toggle(enum led_id id);
#endif
//---------------------------//
// Prototypes end
//---------------------------//



#endif /* USER_IO_INC_USER_IO_DRIVER_H_ */
