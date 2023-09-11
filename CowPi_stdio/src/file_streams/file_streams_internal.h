/**************************************************************************//**
 *
 * @file file_streams_internal.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Functions and structures needed for file streams that do not need to
 *      be exposed outside the library.
 *
 * This file should be `#include`d *only* by .c and .cpp files in the
 * `file_streams` directory
 *
 ******************************************************************************/

/* CowPi_stdio (c) 2022-23 Christopher A. Bohn
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *     http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <Arduino.h>
#include <stdio.h>
#include "typedefs.h"

#ifdef COWPI_STDIO_FILE_STREAMS_INTERNAL

#ifdef __cplusplus
extern "C" {
#endif

// assumes the modulus is a power-of-two
#define INCREMENT_MODULO(value, modulus) (((value) + 1) & ((modulus) - 1))

/**
 * Structure containing everything needed for correct updates to a display
 * module in response to `fprintf`, `cowpi_clear_display`,
 * `cowpi_sleep_display`, or `cowpi_wake_display`.
 */
typedef struct {
#if defined(__AVR__)
    FILE stream;    // for AVR, we'll use the FILE's address to refer back to the stream_data_t, a frequent need
#else
    FILE *stream;   // for ARM, we'll have to compare addresses for occasionally finding a FILE's stream_data_t
#endif //__AVR__
    int (*put)(void *cookie, const char *buffer, int size);
    cowpi_display_module_t display_module;
    cowpi_display_module_protocol_t configuration;
    uint16_t ms_per_signal;
} stream_data_t;

//#define MAXIMUM_NUMBER_OF_STREAMS (5)
//extern int8_t number_of_streams;
//extern stream_data_t streams[MAXIMUM_NUMBER_OF_STREAMS];


/**
 * Structure containing everything needed to place a symbol, or pattern,
 * on a display module from the symbol buffer driven by timer interrupts.
 */
typedef struct {
    void (*callback)(void *symbol); //!< the function that this structure will be passed to; the parameter is considered to be of type `(symbol_t *)`
    stream_data_t *stream_data;     //!< the cookie for the display module
    uint8_t symbol;                 //!< the symbol, or pattern, that will be sent to the display module
    uint8_t symbol_duration;        //!< the length of time between this symbol being sent to the display module and the next symbol being sent to the display module; each unit of timme is 8ms @sa SYMBOL_DURATION_SCALING_FACTOR
} symbol_t;

// BUFFER_SIZE needs to be power-of-two to work with INCREMENT_MODULO
// A larger buffer reduces likelihood of blocking, but ATmega328P has only 2KB of RAM to work with
#if defined(__AVR_ATmega328P__)
#define BUFFER_SIZE (16)
// ATmega4809 has 6KB, and ATmega2560 has 8KB
#elif defined (__AVR_ATmega2560__) || defined (__AVR_ATmega4809__)
#define BUFFER_SIZE (32)
// RA4M1 has 32KB, SAMD21 & nRF52840 have 256KB, and RP24040 has 264KB
#else
#define BUFFER_SIZE (128)
#endif
extern uint8_t buffer_head;
extern uint8_t buffer_tail;
extern symbol_t symbol_buffer[BUFFER_SIZE];
#define BUFFER_IS_EMPTY ((buffer_head) == (buffer_tail))
#define BUFFER_IS_FULL  ((INCREMENT_MODULO(buffer_tail, BUFFER_SIZE)) == (buffer_head))
#define SYMBOL_DURATION_SCALING_FACTOR (8)



/**
 * Given a FILE stream, returns the corresponding `stream_data_t` structure
 * that should be used as a cookie.
 *
 * @param filestream a FILE stream that can be mapped to a cookie
 * @return the `stream_data_t` cookie corresponding to `filestream`
 */
stream_data_t *cowpi_file_to_cookie(FILE *filestream);


/**
 * Adds a symbol to the timer interrupt-driven symbol buffer, to eventually
 * be send to a display module.
 *
 * @param entry the symbol (and related data) that will be sent to the display module
 */
static inline void add_symbol_to_buffer(symbol_t entry) {
    while (BUFFER_IS_FULL) {
        // it's okay that `delay` is blocked while interrupts are disabled,
        // because we can't add something to a full buffer until the timer handler removes something else
        delay(1);
    }
    symbol_buffer[buffer_tail] = entry;
    buffer_tail = INCREMENT_MODULO(buffer_tail, BUFFER_SIZE);
}

/**
 * Places a byte's bits in the reversed order.
 *
 * @param byte the byte to be reversed
 * @return a byte whose bits are in the reverse order as those of the argument
 */
static inline uint8_t reverse_byte(uint8_t byte) {
    byte = ((byte & 0xF0) >> 4) | ((byte & 0x0F) << 4);
    byte = ((byte & 0xCC) >> 2) | ((byte & 0x33) << 2);
    byte = ((byte & 0xAA) >> 1) | ((byte & 0x55) << 1);
    return byte;
}


/**
 * Configures the timer interrupts for the symbol buffer.
 */
void cowpi_enable_buffer_timer(void);


/**
 * @brief The `put` function for a MAX7219-driven 7-segment display module
 *      that does *not* have scrolling text.
 *
 * This function satisfies the `writefn`/`write` argument expected by the
 * newlib/glibc `funopen` and `fopencookie` functions.
 *
 * @param cookie the `stream_data_t` structure with data about the display
 *      module and communication protocol
 * @param buffer an origin buffer containing the characters to be written
 * @param size the number of characters to be written
 * @return the number of characters successfully sent to the display module;
 *      -1 on failure
 */
int cowpi_seven_segment_noscroll_put(void *cookie, const char *buffer, int size);

/**
 * @brief The `put` function for a MAX7219-driven 7-segment display module
 *      that has scrolling text.
 *
 * This function satisfies the `writefn`/`write` argument expected by the
 * newlib/glibc `funopen` and `fopencookie` functions.
 *
 * @param cookie the `stream_data_t` structure with data about the display
 *      module and communication protocol
 * @param buffer an origin buffer containing the characters to be written
 * @param size the number of characters to be written
 * @return the number of characters successfully sent to the display module;
 *      -1 on failure
 */
int cowpi_seven_segment_scrolling_put(void *cookie, const char *buffer, int size);

/**
 * @brief The `put` function for a MAX7219-driven LED matrix display module
 *      that shows scrolling text.
 *
 * This function satisfies the `writefn`/`write` argument expected by the
 * newlib/glibc `funopen` and `fopencookie` functions.
 *
 * @param cookie the `stream_data_t` structure with data about the display
 *      module and communication protocol
 * @param buffer an origin buffer containing the characters to be written
 * @param size the number of characters to be written
 * @return the number of characters successfully sent to the display module;
 *      -1 on failure
 */
int cowpi_led_matrix_scrolling_put(void *cookie, const char *buffer, int size);

/**
 * @brief The `put` function for a HD44780-driven LCD character display module.
 *
 * This function satisfies the `writefn`/`write` argument expected by the
 * newlib/glibc `funopen` and `fopencookie` functions.
 *
 * @param cookie the `stream_data_t` structure with data about the display
 *      module and communication protocol
 * @param buffer an origin buffer containing the characters to be written
 * @param size the number of characters to be written
 * @return the number of characters successfully sent to the display module;
 *      -1 on failure
 */
int cowpi_lcd_character_put(void *cookie, const char *buffer, int size);

/**
 * @brief The `put` function for sending Morse Code to an output pin.
 *
 * This function satisfies the `writefn`/`write` argument expected by the
 * newlib/glibc `funopen` and `fopencookie` functions.
 *
 * @param cookie the `stream_data_t` structure with data about the display
 *      module and communication protocol
 * @param buffer an origin buffer containing the characters to be written
 * @param size the number of characters to be written
 * @return the number of characters successfully sent to the display module;
 *      -1 on failure
 */
int cowpi_morse_code_put(void *cookie, const char *buffer, int size);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_FILE_STREAMS_INTERNAL
