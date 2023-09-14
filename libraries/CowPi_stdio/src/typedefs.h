/**************************************************************************//**
 *
 * @file typedefs.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Structures that consolidate configuration data for display modules
 *      and protocols to communication with the display modules
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

#ifndef COWPI_STDIO_TYPEDEFS_H
#define COWPI_STDIO_TYPEDEFS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Choices of display modules.
 */
enum display_modules {
    NO_MODULE = 0,
    SEVEN_SEGMENT,
    LED_MATRIX,
    LCD_CHARACTER,
    OLED_MATRIX,
    MORSE_CODE
};

/**
 * @brief Rotation/orientations for LED matrices.
 *
 * There is no significance to the names of the orientations except that they
 * are easy-to-remember names for the four possible orientations and make no
 * implications about a preference for how the application programmer orients
 * their LED matrix.
 */
enum orientations {
    EAST = 0,
    NORTH,
    WEST,
    SOUTH
};

/**
 * @brief Mappings the matrix LEDs to each byte's bits.
 *
 * There doesn't appear to be consensus on which end of the matrix corresponds
 * to the most/least-significant bits. Consequently, assuming one mapping could
 * result in the characters being displayed backwards, and so we offer the
 * opportunity to select the mapping.
 *
 * There is no significance to the names of the orientations except that they
 * are easy-to-remember names for the two possible mappings and make no
 * implications about a preference for one mapping or the other.
 */
enum flips {
    HEADS = 0,
    TAILS
};

/**
 * @brief Communication protocols used to send data to a display module.
 */
enum protocols {
    NO_PROTOCOL = 0,
    I2C,
    SPI
};

/**
 * @brief Mappings of transmitted bits to LCD character display lines.
 *
 * Universally, the adapter modules for HD44780-based displays map transmitted
 * bits to some of the display module's lines: four data bits, the RS
 * (data/command) line, the RW (read/write) line, and the backlight.
 * Where they differ is which bits correspond to which lines.
 *
 * The default mapping is that used by the ubiquitous PCF8574-based modules that
 * sell for a couple of dollars. The ADAFRUIT mapping is that used by
 * [Adafruit's I2C/SPI character LCD backpack](https://www.adafruit.com/product/292).
 */
enum adapter_mappings {
    COWPI_DEFAULT = 0,
    ADAFRUIT
};

/**
 * @brief Structure that consolidates all information needed about a display module.
 */
typedef struct {
    enum display_modules display_module;
    uint8_t width;                              // used by all displays except MORSE_CODE
    uint8_t height;                             // used by multi-row displays
    uint8_t words_per_minute;                   // used by timed streams
    enum orientations display_orientation;      // used by LED matrix
    enum flips character_flip;                  // used by LED matrix
} cowpi_display_module_t;

/**
 * @brief Structure that consolidates all information needed to determine how data
 * should be sent to a display module.
 */
typedef struct {
    // C++ doesn't allow anonymous unions, so we'll be a little less memory-efficient
    enum protocols protocol;
    uint8_t data_pin;                       // used by all protocols (including NO_PROTOCOL)
    uint8_t clock_pin;                      // not used by NO_PROTOCOL
    uint8_t select_pin;                     // used only for SPI
    uint8_t i2c_address;                    // used only for I2C
    enum adapter_mappings adapter_mapping;  // used only for HD44780
} cowpi_display_module_protocol_t;


#if defined (ARDUINO_AVR_UNO) || defined (ARDUINO_AVR_NANO) || defined (ARDUINO_AVR_NANO_EVERY) || defined (ARDUINO_SAMD_NANO_33_IOT) || defined (ARDUINO_ARDUINO_NANO33BLE)

#define COWPI_DEFAULT_SPI_DATA_PIN      (11)
#define COWPI_DEFAULT_SPI_CLOCK_PIN     (13)
#define COWPI_DEFAULT_SPI_SELECT_PIN    (10)
#define COWPI_DEFAULT_I2C_DATA_PIN      (18)
#define COWPI_DEFAULT_I2C_CLOCK_PIN     (19)

#elif defined (ARDUINO_AVR_MEGA2560)

#define COWPI_DEFAULT_SPI_DATA_PIN      (51)
#define COWPI_DEFAULT_SPI_CLOCK_PIN     (52)
#define COWPI_DEFAULT_SPI_SELECT_PIN    (10)
#define COWPI_DEFAULT_I2C_DATA_PIN      (20)
#define COWPI_DEFAULT_I2C_CLOCK_PIN     (21)

#elif defined (ARDUINO_RASPBERRY_PI_PICO)

#define COWPI_DEFAULT_SPI_DATA_PIN      (19)
#define COWPI_DEFAULT_SPI_CLOCK_PIN     (18)
#define COWPI_DEFAULT_SPI_SELECT_PIN    (17)
#define COWPI_DEFAULT_I2C_DATA_PIN      (4)
#define COWPI_DEFAULT_I2C_CLOCK_PIN     (5)

#else

#warning No default SPI/I2C pins are defined!

#endif //microcontroller boards


#ifdef __cplusplus
} // extern "C"

// While C++ will allow us to use a struct initializer that uses default (zero)
// values for fields that are not given an explicit initial value, it will not
// allow us to skip over fields when doing so (unlike C) -- for example, we
// cannot initialize i2c_address without explicitly (and unnecessarily)
// initializing select_pin -- and so we'll provide some initialization functions
// for use in C++ code.

/**
 * @brief Produces a `cowpi_display_module_protocol_t` instantiation for a
 * "display" that doesn't require a communication protocol, such as
 * `MORSE_CODE`.
 *
 * @param data_pin the pin used to send data to the "display"
 * @return a `cowpi_display_module_protocol_t` instantiation initialized for
 *      `NO_PROTOCOL`
 */
static inline cowpi_display_module_protocol_t cowpi_configure_single_pin(uint8_t data_pin) {
    return (cowpi_display_module_protocol_t) {
            .protocol = NO_PROTOCOL,
            .data_pin = data_pin,
            .clock_pin = 0,
            .select_pin = 0,
            .i2c_address = 0,
            .adapter_mapping = COWPI_DEFAULT
    };
}

/**
 * @brief Produces a `cowpi_display_module_protocol_t` instantiation for the
 * Serial-Parallel Interface (SPI) protocol.
 *
 * @param data_pin the data output pin, possibly called SDO, COPI, PICO,
 *      DAT (data), TX, or (deprecated) MOSI
 * @param clock_pin the clock pin, possibly called SCK or CLK
 * @param select_pin the peripheral-select pin, possibly called CS, LAT (latch),
 *      or (deprecated) SS
 * @param adapter_mapping (optional) the mapping of transmitted bits to HD44780
 *      lines
 * @return a `cowpi_display_module_protocol_t` instantiation initialized for
 *      `SPI`
 */
static inline cowpi_display_module_protocol_t cowpi_configure_spi(uint8_t select_pin,
                                                                  uint8_t data_pin = COWPI_DEFAULT_SPI_DATA_PIN,
                                                                  uint8_t clock_pin = COWPI_DEFAULT_SPI_CLOCK_PIN,
                                                                  enum adapter_mappings adapter_mapping = COWPI_DEFAULT) {
    return (cowpi_display_module_protocol_t) {
            .protocol = SPI,
            .data_pin = data_pin,
            .clock_pin = clock_pin,
            .select_pin = select_pin,
            .i2c_address = 0,
            .adapter_mapping = adapter_mapping
    };
}

/**
 * @brief Produces a `cowpi_display_module_protocol_t` instantiation for the
 * Inter-Integrated Circuit (I2C, or IIC), aka the Two-Wire Interface (TWI),
 * protocol.
 *
 * @param data_pin the data output pin, SDA
 * @param clock_pin the clock pin, SCL
 * @param i2c_address the peripheral's 7-bit address
 * @param adapter_mapping (optional) the mapping of transmitted bits to HD44780
 *      lines
 * @return a `cowpi_display_module_protocol_t` instantiation initialized for
 *      `I2C`
 */
static inline cowpi_display_module_protocol_t cowpi_configure_i2c(uint8_t i2c_address,
                                                                  uint8_t data_pin = COWPI_DEFAULT_I2C_DATA_PIN,
                                                                  uint8_t clock_pin = COWPI_DEFAULT_I2C_CLOCK_PIN,
                                                                  enum adapter_mappings adapter_mapping = COWPI_DEFAULT) {
    return (cowpi_display_module_protocol_t) {
            .protocol = I2C,
            .data_pin = data_pin,
            .clock_pin = clock_pin,
            .select_pin = 0,
            .i2c_address = i2c_address,
            .adapter_mapping = adapter_mapping
    };
}

/**
 * @brief Produces a `cowpi_display_module_t` instantiation for a
 * MAX7219-controlled 8-digit/7-segment display module.
 *
 * @return a `cowpi_display_module_t` instantiation initialized for an
 *      8-digit/7-segment display
 */
static inline cowpi_display_module_t cowpi_configure_seven_segment_display() {
    return (cowpi_display_module_t) {
            .display_module = SEVEN_SEGMENT,
            .width = 8,
            .height = 1,
            .words_per_minute = 0,
            .display_orientation = EAST,
            .character_flip = HEADS
    };
}

/**
 * @brief Produces a `cowpi_display_module_t` instantiation for a
 * MAX7219-controlled 8-digit/7-segment display module that has text scrolling
 * from right-to-left.
 *
 * @param words_per_minute the approximate rate at which the text scrolls across
 *      the display
 * @return a `cowpi_display_module_t` instantiation initialized for an
 *      8-digit/7-segment display with scrolling text
 */
static inline cowpi_display_module_t cowpi_configure_scrolling_seven_segment_display(uint8_t words_per_minute = 30) {
    return (cowpi_display_module_t) {
            .display_module = SEVEN_SEGMENT,
            .width = 8,
            .height = 1,
            .words_per_minute = words_per_minute,
            .display_orientation = EAST,
            .character_flip = HEADS
    };
}

/**
 * @brief Produces a `cowpi_display_module_t` instantiation for a
 * MAX7219-controlled 8x8 LED matrix display module that has text scrolling from
 * right-to-left.
 *
 * You may need to experiment with choices of `display_orientation` and
 * `character_flip` -- one of the eight possible combinations will match your
 * particular display.
 *
 * @param words_per_minute the approximate rate at which the text scrolls across
 *      the display
 * @param display_orientation parameter to match the characters' rotation with
 *      that of your display
 * @param character_flip parameter to match the characters' bit order with that
 *      of your display
 * @return a `cowpi_display_module_t` instantiation initialized for an
 *      8x8 LED matrix display with scrolling text
 */
static inline cowpi_display_module_t cowpi_configure_led_matrix_display(uint8_t words_per_minute = 30,
                                                                        enum orientations display_orientation = EAST,
                                                                        enum flips character_flip = HEADS) {
    return (cowpi_display_module_t) {
            .display_module = LED_MATRIX,
            .width = 8,
            .height = 8,
            .words_per_minute = words_per_minute,
            .display_orientation = display_orientation,
            .character_flip = character_flip
    };
}

/**
 * @brief Produces a `cowpi_display_module_t` instantiation for an
 * HD44780-controlled LCD character display.
 *
 * @param width the number of characters per row; 16 is the default, and 20 is
 *      an option
 * @param height the number of rows; 2 is the default, and 4 is an option
 * @return a `cowpi_display_module_t` instantiation initialized for an
 *      LCD character display
 */
static inline cowpi_display_module_t cowpi_configure_lcd_character_display(uint8_t width = 16, uint8_t height = 2) {
    return (cowpi_display_module_t) {
            .display_module = LCD_CHARACTER,
            .width = width,
            .height = height,
            .words_per_minute = 0,
            .display_orientation = EAST,
            .character_flip = HEADS
    };
}

/**
 * @brief Produces a `cowpi_display_module_t` instantiation for a "display" of
 * that places Morse Code on an output pin.
 *
 * The nominal usage is to illuminate an LED; however, a buzzer with its own
 * oscillator crystal will also work.
 *
 * @param words_per_minute the approximate rate at which the text is sent to
 *      the output pin
 * @return a `cowpi_display_module_t` instantiation initialized for sending
 *      Morse Code to an output pin
 */
static inline cowpi_display_module_t cowpi_configure_morse_code(uint8_t words_per_minute = 5) {
    return (cowpi_display_module_t) {
            .display_module = MORSE_CODE,
            .width = 0,
            .height = 0,
            .words_per_minute = words_per_minute,
            .display_orientation = EAST,
            .character_flip = HEADS
    };
}

#endif //__CPLUSPLUS

#endif //COWPI_STDIO_TYPEDEFS_H
