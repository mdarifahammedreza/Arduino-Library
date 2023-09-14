/**************************************************************************//**
 *
 * @file display_module_streams.c
 *
 * @brief Functions that generate and use `FILE` streams for display
 *      modules.
 *
 * Implements the central `cowpi_add_display_module` function that generates
 * FILE streams which can be used with `fprintf`. These FILE streams can also
 * used to perform common manipulations of the display modules using other
 * public functions implemented in this file.
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

#define COWPI_STDIO_FILE_STREAMS_INTERNAL

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <Arduino.h>
#include "file_streams.h"
#include "file_streams_internal.h"
#include "../hd44780/hd44780.h"
#include "../max7219/max7219.h"
#include "../translations/translations.h"



/* Unlike the setup in the monolithic CowPi library (through v0.4.1), the 
 * compiler can't conclude when the custom-defined fonts aren't in use.
 * I'm not worried about the 7-segment font -- that's only 128 bytes for the
 * font plus some reasonable amount for the `put` function. However, eliminating
 * the Morse Code font (and its `put`) shaves a little over 1KB, and eliminating
 * the Matrix font (plus the LED Matrix `put`) shaves well over 2KB. */
#if defined (__AVR_ATmega328P__)

#if !defined(MORSE_FONT)
#define NO_MORSE_FONT
#endif //MORSE_FONT
#if !defined(MATRIX_FONT)
#define NO_MATRIX_FONT
#endif //MATRIX_FONT

#else

#if !defined(NO_MORSE_FONT)
#define MORSE_FONT
#endif //NO_MORSE_FONT
#if !defined(NO_MATRIX_FONT)
#define MATRIX_FONT
#endif //NO_MATRIX_FONT

#endif //Allowing/prohibiting memory-expensive display modules



#define MAXIMUM_NUMBER_OF_STREAMS (5)
int8_t number_of_streams;
stream_data_t streams[MAXIMUM_NUMBER_OF_STREAMS];

#if defined(__AVR__)

/**
 * @brief The `put` function that AVR-libc expects for output FILE streams.
 *
 * This function is a wrapper for the more-generally-defined `put` function that
 * glibc and newlib expect for output FILE streams. This function satisfies the
 * AVR-libc `put` interface but simply calls the appropriate display module's
 * newlib `put` function.
 *
 * *n.b.*, This function satisfies the `put` argument expected by `fdevopen` and
 * `fdev_setup-stream` -- specifically, it returns 0 on success. This is in
 * contrast with AVR-libc's `putc` and `fputc` which have the same signature but
 * return the character on success.
 *
 * @param c the character to be displayed
 * @param filestream the FILE stream for the display module
 * @return 0 if the character is successfully sent to the display module;
 *      non-zero otherwise
 */
static int cowpi_display_module_put(char c, FILE *filestream);

#endif //__AVR__

FILE *cowpi_add_display_module(cowpi_display_module_t display_module, cowpi_display_module_protocol_t configuration) {
    if (number_of_streams == MAXIMUM_NUMBER_OF_STREAMS) return NULL;
    FILE *stream;
    stream_data_t *stream_data = streams + number_of_streams;
    memcpy(&stream_data->display_module, &display_module, sizeof(cowpi_display_module_t));
    memcpy(&stream_data->configuration, &configuration, sizeof(cowpi_display_module_protocol_t));
    uint8_t words_per_minute = display_module.words_per_minute;
    switch (stream_data->configuration.protocol) {
        case NO_PROTOCOL:
            if (display_module.display_module != MORSE_CODE) return NULL;
            cowpi_pin_mode(stream_data->configuration.data_pin, OUTPUT);
            break;
        case SPI:
            if ((stream_data->configuration.data_pin == 0)
                && (stream_data->configuration.data_pin == stream_data->configuration.select_pin)) {
                stream_data->configuration.select_pin = COWPI_DEFAULT_SPI_SELECT_PIN;
            }
            if ((stream_data->configuration.data_pin == 0)
                && (stream_data->configuration.data_pin == stream_data->configuration.clock_pin)) {
                stream_data->configuration.data_pin = COWPI_DEFAULT_SPI_DATA_PIN;
                stream_data->configuration.clock_pin = COWPI_DEFAULT_SPI_CLOCK_PIN;
            }
            cowpi_pin_mode(stream_data->configuration.data_pin, OUTPUT);
            cowpi_pin_mode(stream_data->configuration.clock_pin, OUTPUT);
            cowpi_pin_mode(stream_data->configuration.select_pin, OUTPUT);
            break;
        case I2C:
            if ((stream_data->configuration.data_pin == 0)
                && (stream_data->configuration.data_pin == stream_data->configuration.clock_pin)) {
                stream_data->configuration.data_pin = COWPI_DEFAULT_I2C_DATA_PIN;
                stream_data->configuration.clock_pin = COWPI_DEFAULT_I2C_CLOCK_PIN;
            }
            cowpi_pin_mode(stream_data->configuration.data_pin, INPUT);
            cowpi_pin_mode(stream_data->configuration.clock_pin, INPUT);
            if (!stream_data->configuration.i2c_address) return NULL;           // general call
            if (stream_data->configuration.i2c_address & 0x80) return NULL;     // invalid address
            break;
        default:
            return NULL;
    }
    switch (display_module.display_module) {
        case SEVEN_SEGMENT:
            // default to a single module of 8 digits (which is good because we're not handling chained modules yet)
            if (!stream_data->display_module.width) stream_data->display_module.width = 8;
            // must use SPI
            if (stream_data->configuration.protocol != SPI) return NULL;
            // number of digits must be multiple of 8
            if (display_module.width & 0x7) return NULL;
            cowpi_setup_max7219(&stream_data->configuration);
            // if 0wpm (the default), no scrolling
            if (!words_per_minute) {
                stream_data->put = cowpi_seven_segment_noscroll_put;
#if !defined (NO_TIMED_DISPLAYS)
            } else {
                cowpi_enable_buffer_timer();
                // minimum 6wpm (2000ms per letter), maximum 255wpm (47ms per letter) when scrolling
                if (words_per_minute < 6) words_per_minute = 6;
                stream_data->ms_per_signal = 60000 /* ms per minute */ / 5 /* letters per "PARIS" word */ / words_per_minute;
                stream_data->put = cowpi_seven_segment_scrolling_put;
#endif //NO_TIMED_DISPLAYS
            }
            break;
#if defined (MATRIX_FONT) && !defined (NO_TIMED_DISPLAYS)
        case LED_MATRIX:
            cowpi_enable_buffer_timer();
            // default to a single 8x8 module (which is good because we're not handling chained modules yet)
            if (!stream_data->display_module.width) stream_data->display_module.width = 8;
            if (!stream_data->display_module.height) stream_data->display_module.height = 8;
            // must use SPI
            if (stream_data->configuration.protocol != SPI) return NULL;
            // number of rows and columns must each be multiple of 8
            if (display_module.width & 0x7) return NULL;
            if (display_module.height & 0x7) return NULL;
            cowpi_setup_max7219(&stream_data->configuration);
            // calculate wpm assuming fixed spacing of 6 columns per character (5 character + 1 intercharacter space)
            // proportional spacing 26 columns per "PARIS" word (vs 30) -- a non-trivial but acceptable error
            // default to 5wpm
            if (!words_per_minute) words_per_minute = 5;
            // minimum 1wpm (250ms per column), maximum 250wpm (1ms per column)
            if (words_per_minute > 250) words_per_minute = 250;
            stream_data->ms_per_signal = 60000 /* ms per minute */ / 30 /* columns per "PARIS" word */ / words_per_minute;
            stream_data->put = cowpi_led_matrix_scrolling_put;
            break;
#endif //MATRIX FONT
        case LCD_CHARACTER:
            // default to LCD1602
            if (!stream_data->display_module.width) stream_data->display_module.width = 16;
            if (!stream_data->display_module.height) stream_data->display_module.height = 2;
            // must be LCD1602 or LCD2004
            if (!(stream_data->display_module.width == 16 && stream_data->display_module.height == 2)
                && !(stream_data->display_module.width == 20 && stream_data->display_module.height == 4))
                return NULL;
            cowpi_setup_hd44780(&stream_data->configuration);
            cowpi_hd44780_set_backlight(&stream_data->configuration, 1);
            stream_data->put = cowpi_lcd_character_put;
            break;
#if defined (MORSE_FONT) && !defined (NO_TIMED_DISPLAYS)
        case MORSE_CODE:
            cowpi_enable_buffer_timer();
            // default to 5wpm
            if (!words_per_minute) words_per_minute = 5;
            // minimum 1wpm (1200ms per unit), maximum 150wpm (1ms per unit)
            if (words_per_minute > 150) words_per_minute = 150;
            stream_data->ms_per_signal = 60000 /* ms per minute */ / 50 /* units per "PARIS" word */ / words_per_minute;
            stream_data->put = cowpi_morse_code_put;
            break;
#endif //MORSE_FONT
        default:
            return NULL;
    }
#if defined(__AVR__)
    stream = &(streams[number_of_streams].stream);
    fdev_setup_stream(stream, cowpi_display_module_put, NULL, _FDEV_SETUP_WRITE);
#elif defined (ARDUINO_ARCH_SAMD) || defined (__MBED__)
    stream = funopen(stream_data, NULL, stream_data->put, NULL, NULL);
    // I suppose we should make line buffering an option, but not today
    if (setvbuf(stream, NULL, _IONBF, 0)) return NULL;
    stream_data->stream = stream;
#else
    stream = NULL;
#endif //architecture
    number_of_streams++;
    return stream;
}

#if defined(__AVR__)

static int cowpi_display_module_put(char c, FILE *filestream) {
    stream_data_t *stream_data = cowpi_file_to_cookie(filestream);
    int returned_value = stream_data->put(stream_data, &c, 1);
    if (returned_value == -1 || returned_value == 0) {
        return -1;
    } else {
        return 0;
    }
}

#endif //__AVR__

stream_data_t *cowpi_file_to_cookie(FILE *filestream) {
#ifdef __AVR__
    // because the file stream is the first element in the struct,
    // the address of that element is also the base address of the struct
    return (stream_data_t *) filestream;
#else
    stream_data_t *cookie = NULL;
    int i = 0;
    while (cookie == NULL && i < MAXIMUM_NUMBER_OF_STREAMS) {
        if (streams[i].stream == filestream) {
            cookie = streams + i;
        } else {
            i++;
        }
    }
    return cookie;
#endif //__AVR__
}

void cowpi_clear_display(FILE *filestream) {
    stream_data_t *stream_data = cowpi_file_to_cookie(filestream);
    int (*put)(void *, const char *, int) = stream_data->put;
    cowpi_display_module_protocol_t *configuration = &stream_data->configuration;
    if (put == cowpi_seven_segment_noscroll_put) {
        fprintf(filestream, "\r\n");    // place cursor at start of line, and then \n clears it
    } else if (put == cowpi_lcd_character_put) {
        fprintf(filestream, "\f");      // so that `put` knows where the cursor is
        cowpi_hd44780_clear_display(configuration);
    } else {}
}

void cowpi_sleep_display(FILE *filestream) {
    stream_data_t *stream_data = cowpi_file_to_cookie(filestream);
    int (*put)(void *, const char *, int) = stream_data->put;
    cowpi_display_module_protocol_t *configuration = &stream_data->configuration;
    if (put == cowpi_seven_segment_noscroll_put || put == cowpi_seven_segment_scrolling_put) {
        cowpi_max7219_shutdown(configuration);
    } else if (put == cowpi_lcd_character_put) {
        cowpi_hd44780_set_backlight(configuration, false);
    } else {}
}

void cowpi_wake_display(FILE *filestream) {
    stream_data_t *stream_data = cowpi_file_to_cookie(filestream);
    int (*put)(void *, const char *, int) = stream_data->put;
    cowpi_display_module_protocol_t *configuration = &stream_data->configuration;
    if (put == cowpi_seven_segment_noscroll_put || put == cowpi_seven_segment_scrolling_put) {
        cowpi_max7219_startup(configuration);
    } else if (put == cowpi_lcd_character_put) {
        cowpi_hd44780_set_backlight(configuration, true);
    } else {}
}
