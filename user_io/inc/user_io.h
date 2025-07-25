/**
 *
 * @file user_io.h
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

#ifndef USER_IO_INC_USER_IO_H_
#define USER_IO_INC_USER_IO_H_



//---------------------------//
// Include begin
//---------------------------//
#include <stdint.h>
#include <stdbool.h>
#include "user_io_config.h"
//---------------------------//
// Include end
//---------------------------//



//---------------------------//
// Prototypes begin
//---------------------------//
void user_io_init(void);
void user_io_irq_handler(void);



#ifdef SWITCHES_USE
bool switch_on(enum switch_id id);
bool switch_off(enum switch_id id);
#endif



#ifdef BTNS_USE
bool btn_hold_ms(enum btn_id id, uint16_t ms);
bool btn_click(enum btn_id id);
bool btn_released(enum btn_id id);
bool btn_depressed(enum btn_id id);
bool btns_no_input_ms(uint32_t idle_ms);
#endif



#ifdef LEDS_USE
void led_blink_infinite(enum led_id id, uint16_t blink_rate_ms);
void led_blink_ms(enum led_id id, uint16_t blink_rate_ms, uint16_t duration_ms);
void led_blink_n_times(enum led_id id, uint16_t blink_rate_ms, uint16_t n);
void led_on(enum led_id id);
void led_off(enum led_id id);
void led_force_off(enum led_id id);
void led_pulse(enum led_id id, uint16_t pulse_duration_ms);

void led_all_off(void);
void led_all_force_off(void);
void led_all_on(void);
void led_all_blink_infinite(uint16_t blink_rate_ms);
void led_all_blink_n_times(uint16_t blink_rate_ms, uint16_t n);
void led_all_blink_ms(uint16_t blink_rate_ms, uint16_t duration_ms);
void led_all_pulse(uint16_t pulse_duration_ms);
#endif



#ifdef INTERVALS_USE
bool interval_reached_ms(enum interval_id id, uint32_t ms);
#endif
//---------------------------//
// Prototypes end
//---------------------------//



#endif /* USER_IO_INC_USER_IO_H_ */
