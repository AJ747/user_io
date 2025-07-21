# About User IO - Porting guide below

User IO is a library for reading buttons, controlling LEDs and running code with fixed intervals in a non-blocking way for embedded applications written in C. Add up to 256 buttons, LEDs and "intervals" to your project and control them easily using User IO. You can also choose which features you want to use, and which to exclude, see [step 4 - Update macro-parameters](#4-Update-macro-parameters) in the porting guide.

It needs a timer to trigger its handler every *x* milliseconds to function properly.

> [!NOTE]
> It's recommended to call the handler a minimum of every 10 ms. Calling it quicker than this will result in poor system performance. 

----

#### Features for LED-control
##### Control Individually:
* On - `led_on()`
* Off
    * Off - `led_off()`
    * Force off - `led_force_off()`
* Pulse (custom on-time) - `led_pulse()` 
* Blink (custom period)
    * Indefinitely - `led_blink_infinite()`
    * *n* times - `led_blink_n_times()`
    * For *x* milliseconds - `led_blink_ms()`

##### Controll all simultaneously:
* All on - `led_all_on()`
* All off
    * Off - `led_all_off()`
    * Force off - `led_all_force_off()`
* All pulse (custom on-time) - `led_all_pulse()` 
* All blink (custom period)
    * Indefinitely - `led_all_blink_infinite()`
    * *n* times - `led_all_blink_n_times()`
    * For *x* milliseconds - `led_all_blink_ms()`

> [!IMPORTANT]
> If a LED is set to a new effect when an ongoing effect hasn't finished, then the ongoing effect will be discarded.

> [!WARNING]
> `led_off()` and `led_all_off()` only works for effects lasting indefinitely. Does not work with effects lasting a finite time as these automatically turn off when the effect is done.

> [!TIP]
> Use `led_force_off()` and `led_all_force_off()` instead, this will force the LED to turn off no matter what effect is currently ongoing. 

----

#### Features for button-sensing:
* Sense click - `btn_click()`
* Sense hold (custom threshold) - `btn_hold_ms()`
* Sense release after click/hold - `btn_released()`
* Sense if depressed - `btn_depressed()`
* All buttons debounced (custom threshold)

----

#### Features for "intervals":

An "interval" enables you to run code at a specific interval in a non-blocking way, an example is shown below:

```C
int main(void) {
	// Init
	system_init();

	while (1) {
		...
		
		// Prints every 1000 ms, 1 sec, in a non-blocking way
		if (interval_reached_ms(INTERVAL0, 1000)) {
			printf("Hello, World!");
		}

		...
	}
}
```

> [!IMPORTANT]
> An "interval" counter can run up to ~49 days, 2^32 ms, before overflowing.

----

# Porting guide

A quick guide on how to properly port the User IO library to any embedded platform. Here you must open some source-files and edit lines, I've added `// <-- EDIT HERE` on every line you must edit. 

> [!NOTE]
> A dedicated timer is needed that triggers an interrupt every *x* milliseconds.

<br>

> [!IMPORTANT]
> Button, LED-states and "intervals" are all updated in the TIMx IRQ handler, this can take some time. Advised to use lowest IRQ priority for TIMx if possible. 
>
> See [step 2 - Timer setup (Alternative method)](#2-timer-setup-alternative-method) for an alternative method where we set a simple flag in the TIMx IRQ handler and run the User IO handler in the main-loop.
>
> This will cause timing to always be off by a certain amount depending on what's in the main loop.
----

## Steps:

#### 1. Init library
Include `user_io.h` and call ```user_io_init()``` in the setup section.

```C
#include "user_io.h"

int main(void) {
    // Setup section
    user_io_init();

    ...
}
```

<br>

#### 2. Timer setup (Normal method)
Init a timer that calls an interrupt handler every *x* milliseconds. This depends on how responsive you want the buttons, LEDs and "intervals" to be. A minimum of *x* = 10 ms should be more than fast enough. 

Here you must add ```user_io_irq_handler()``` in the interrupt handler. In our case, we are using TIMx with a period of 10 ms.

```C
void TIMx_IRQHandler(void) {
	// TIMx overflow IRQ
	if (TIMx_OVR()) {
		user_io_irq_handler();		
		
		// Clear flag
		CLEAR_TIMx_OVR();
	}
}
```

> [!IMPORTANT]
> Advised to configure TIMx IRQ handler with the lowest possible priority.

<br>

#### 2. Timer setup (Alternative method)
Init a timer that calls an interrupt handler every *x* milliseconds. This depends on how responsive you want the buttons, LEDs and "intervals" to be. A minimum of *x* = 10 ms should be more than fast enough.

First, go to `user_io_config.h` and uncomment `#define ALTERNATIVE_IRQ_METHOD`.

Second, you must set `user_io_handle_rdy` in the interrupt handler when an overflow occurs.

Third, in the main program, you must check if `user_io_handle_rdy` is set and call `user_io_irq_handler()`, then reset `user_io_handle_rdy`. 

Full example:
```C
In `user_io_config.h`

// Comment out if alternative method is used
#define ALTERNATIVE_IRQ_METHOD

In `irq_handler.c`

extern user_io_handle_rdy;

void TIMx_IRQHandler(void) {
	// TIMx overflow IRQ
	if (TIMx_OVR()) {
        // Set flag
        user_io_handle_rdy = true;
        
        // Clear flag
        CLEAR_TIMx_OVR();
	}
}

In `main.c`

extern user_io_handle_rdy; 

int main(void) {
    ...

    while(1) {
        ...

        if (user_io_handle_rdy) {
            // Update states
            user_io_irq_handle();

            // Clear flag
            user_io_handle_rdy = false;
        }

        ...
    }
}
```

> [!CAUTION]
> Since the alternative method is used, there will not be a fixed interval between updating states. This means that timing will always be off by a certain amount depending on what's in the main loop.

<br>

#### 3.1 Choose amount of buttons, LEDs and "intervals"
Choose the amount of buttons and LEDs used by opening `user_io_config.h` and adding these to `enum led_id`, `enum btn_id` and `enum interval_id` as follows:

```C
enum btn_id {
	BTN0 = 0,  	// <-- EDIT HERE
	BTN1,  		// <-- EDIT HERE
	BTN2,  		// <-- EDIT HERE
};

enum led_id {
	LED0 = 0,  	// <-- EDIT HERE
	LED1,  		// <-- EDIT HERE
	LED2  		// <-- EDIT HERE
};

enum interval_id {
	INTERVAL0 = 0,	// <-- EDIT HERE
	INTERVA1,	// <-- EDIT HERE
	INTERVA2  	// <-- EDIT HERE
};
```

Here we have 3 buttons, 3 LEDs and 3 "intervals".

<br>

#### 3.2 Init pins

Open `user_io_driver.c` and find `btn_pins_init()` and `led_pins_init()`. Here you must add code to init GPIO pins used for buttons and LEDs, then turn all LEDs off.

> [!NOTE]
> Recommended to use internal pull-ups on buttons if available, results in easier PCB routing if ground plane is used.

```C
void btn_pins_init(void) {
    // Button pins as output with internal pull-up
    PIN_CONFIG(BTN0_PIN, INPUT_PULL_UP); // <-- EDIT HERE
    PIN_CONFIG(BTN1_PIN, INPUT_PULL_UP); // <-- EDIT HERE
    PIN_CONFIG(BTN2_PIN, INPUT_PULL_UP); // <-- EDIT HERE
}

void led_pins_init(void) {
    // LED pins as output
	PIN_CONFIG(LED0_PIN, OUTPUT); // <-- EDIT HERE
	PIN_CONFIG(LED1_PIN, OUTPUT); // <-- EDIT HERE
	PIN_CONFIG(LED2_PIN, OUTPUT); // <-- EDIT HERE

	// LEDs off
	PIN_LOW(LED0_PIN); // <-- EDIT HERE
	PIN_LOW(LED1_PIN); // <-- EDIT HERE
	PIN_LOW(LED2_PIN); // <-- EDIT HERE
}
```

<br>

#### 3.3 Modify drivers

Open `user_io_driver.c` and find `btn_get_state()`,  `led_driver_on()`, `led_driver_off()` and `led_driver_toggle()`. Add code to set, reset and toggle the pins as shows below:

> [!IMPORTANT]
> If internal pull-ups are used, remember to invert when polling the button-pin as shown below. Meaning, use `!PIN_READ(BTNx_PIN);` instead of `PIN_READ(BTNx_PIN);`.

```C
enum btn_state btn_get_state(enum btn_id id) {
	switch (id) {
		case BTN0: 
			return !PIN_READ(BTN0_PIN); // <-- EDIT HERE

		case BTN1: 
			return !PIN_READ(BTN1_PIN); // <-- EDIT HERE
			
		case BTN2: 
			return !PIN_READ(BTN2_PIN); // <-- EDIT HERE
			
		default:
			return DEPRESSED;
	}
}

void led_driver_on(enum led_id id) {
	switch (id) {
		case LED0:
			PIN_HIGH(LED0_PIN); // <-- EDIT HERE
			break;

		case LED1:
			PIN_HIGH(LED1_PIN); // <-- EDIT HERE
			break;

		case LED2:
			PIN_HIGH(LED2_PIN); // <-- EDIT HERE
			break;
	}
}

void led_driver_off(enum led_id id) {
	switch (id) {
		case LED0:
			PIN_LOW(LED0_PIN);	// <-- EDIT HERE
			break;
			
		case LED1:
			PIN_LOW(LED1_PIN);	// <-- EDIT HERE
			break;

		case LED2:
			PIN_LOW(LED2_PIN);	// <-- EDIT HERE
			break;
	}
}

void led_driver_toggle(enum led_id id) {
	switch (id) {
		case LED0:
			PIN_TOGGLE(LED0_PIN); // <-- EDIT HERE
			break;
			
		case LED1:
			PIN_TOGGLE(LED1_PIN); // <-- EDIT HERE
			break;

		case LED2:
			PIN_TOGGLE(LED2_PIN); // <-- EDIT HERE
			break;
	}
}
```

If you are using more or less buttons and LEDs than shown here, then you must add `case BTNx:` or `case LEDx:` to each respective function.

<br>

#### 4. Update macro-parameters

Open `user_io_config.h` and find the `#define` section, then alter the following:

```C
/// Uncomment if alternative method is used
//#define ALTERNATIVE_IRQ_METHOD // <-- EDIT HERE

// Comment if feature is not needed
#define BTNS_USE // <-- EDIT HERE
#define LEDS_USE // <-- EDIT HERE
#define INTERVALS_USE // <-- EDIT HERE

// IRQ handler is called every 10 ms // <-- EDIT HERE
#define USER_IO_HANDLER_PERIOD_MS TIMx_PERIOD_MS // <-- EDIT HERE

// Button sampling time, alter if faster clicking is required
#define BTN_DEBOUNCE_TRESHOLD_MS 20 // <-- EDIT HERE

#define BTNS_AMOUNT	3 // <-- EDIT HERE
#define LEDS_AMOUNT	3 // <-- EDIT HERE
#define INTERVALS_AMOUNT 3 // <-- EDIT HERE
```

<br>

#### 5. A simple program

A simple program to try out some features:
```C
#include "user_io.h"

int main(void) {
    // Setup
    system_setup();
    ...
    user_io_init();

    while (1) {
        // Do different effects if held down more than 1000 ms
        if (btn_hold_ms(BTN0, 1000)) {
            // Blink, on and off every 100 ms
            led_blink_infinite(LED0, 100);

            // Blink 10 times, on and off every 100 ms
            led_blink_n_times(LED1, 100, 10);

            // Blink for 1 sec, on and off every 100 ms
            led_blink_ms(LED2, 100, 1000);
            
        // Pulse LED if click
        } else if (btn_click(BTN0)) {
            // LED on for 1 sec, then turn off
            led_pulse(LED0, 1000);

        // Turn off all LEDs if not pressed
        } else {
            led_all_off();
        }

	// Prints every 1000 ms 
	if (interval_reached_ms(INTERVAL0, 1000)) {
		printf("Hello, World!");
	}
    }
}

void TIMx_IRQHandler(void) {
	// TIMx overflow IRQ
	if (TIMx_OVR()) {
		user_io_irq_handler();		
		
		// Clear flag
		CLEAR_TIMx_OVR();
	}
}
```

> [!NOTE]
> For more documentation, see the Doxygen folder. Path is "doxygen/html/index.html".

 









