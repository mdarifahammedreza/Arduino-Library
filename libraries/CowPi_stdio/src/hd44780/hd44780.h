/**************************************************************************//**
 *
 * @file hd44780.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Utility functions for peripherals driven by HD44780 such as
 *      16x2 and 20x4 LCD character display modules.
 *
 * The HD44780 will initially be placed in 4-bit mode, and:
 * - With each character, increment the cursor's location but do not shift the
 *   display
 * - Turn the display on, the cursor off, and no blinking
 * - Clear the display
 *
 * The HD44780 is complex enough that the firmware program should call
 * `cowpi_hd44780_send_command()`, `cowpi_hd44780_send_character()`, or another
 * more-specific function. These functions will take care of dividing the data
 * into halfbytes, managing timing, and other complexities, and will call
 * `cowpi_hd44780_send_halfbyte()`.
 *
 * Unlike the MAX7219 and the SSD1306, the HD44780 does not handle serial
 * communication on its own and requires a serial adapter. The mapping between
 * the byte sent from the program and the lines on the display module is
 * specified by the `adapter_mapping` field in the functions'
 * `cowpi_display_module_protocol_t` argument
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

#ifndef COWPI_STDIO_HD44780_H
#define COWPI_STDIO_HD44780_H

#include <stdbool.h>
#include <stdint.h>
#include "../typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Pointer to function that sends a halfbyte to the HD44780 character
 * display.
 *
 * The `cowpi_hd44780` utility functions all make use of the
 * `cowpi_hd44780_send_halfbyte()` function pointer.
 *
 * Initially, `cowpi_hd44780_send_halfbyte()` is one of two default
 * implementations, depending on whether the communication protocol is SPI or
 * I2C.
 *
 * Re-implementing this function to use SPI or I2C hardware is a possible
 * part of a memory-mapped I/O lab assignment.
 *
 * SPI: Assumes the display module's data-in line is connected to the
 * microcontroller's COPI pin (D11 on most Arduino boards), the display module's
 * serial-clock line is connected to the microcontroller's SCK pin (D13 on most
 * Arduino boards), and the display module's chip-select line is connected to
 * Arduino pin D10.
 *
 * I2C: Assumes the display module's data line is connected to the
 * microcontroller's SDA pin (D18 on most Arduino boards) and the display
 * module's serial-clock line is connected to the microcontroller's SCL pin (D19
 * on most Arduino boards).
 *
 * @sa cowpi_hd44780_send_halfbyte
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 * @param halfbyte the data to be sent to the display module
 * @param is_command indicates whether the data is part of a command (`true`)
 *      or part of a character (`false`)
 */
typedef void (*cowpi_hd44780_send_halfbyte_t)(const cowpi_display_module_protocol_t *configuration,
                                              uint8_t halfbyte, bool is_command);

/**
 * @brief Configures the HD44780 to nominal settings
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 */
void cowpi_setup_hd44780(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Reassigns the `cowpi_hd44780_send_halfbyte` function pointer to point
 *      to the specified function.
 *
 * During setup, this function is used to assign one of the two default
 * `cowpi_hd44780_send_halfbyte_t` implementations to
 * `cowpi_hd44780_send_halfbyte`, unless it was previously used to assign a
 * re-implementation. It can also later be used to assign a re-implementation.
 *
 * @deprecated Make assignments directly to cowpi_hd44780_send_halfbyte
 *
 * @param send_halfbyte_function the function to be used to send halfbytes to
 *      the HD44780 character display
 */
void cowpi_hd44780_set_send_halfbyte_function(cowpi_hd44780_send_halfbyte_t send_halfbyte_function);

/**
 * @brief Places the specified character on the display at the specified
 * location.
 *
 * The character is an ASCII or "extended-ASCII" character, or a custom
 * character created by using `cowpi_hd44780_create_character()`.
 *
 * Note that the base address of the top row is 0x00, and the base address of
 * the bottom row is 0x40.
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 * @param address the address of the desired location
 * @param character the character to be displayed
 */
void cowpi_hd44780_place_character(const cowpi_display_module_protocol_t *configuration,
                                   uint8_t address, uint8_t character);

/**
 * @brief Places the cursor at the specified location without updating the
 * display.

 * While the cursor will move to the specified location, it will only be visibly
 * there if the cursor is on.
 *
 * Note that the base address of the top row is 0x00, and the base address of
 * the bottom row is 0x40.
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 * @param address the address of the desired location
 */
void cowpi_hd44780_place_cursor(const cowpi_display_module_protocol_t *configuration, uint8_t address);

/**
 * @brief Sends a command to the HD44780 character display
 *
 * The command is a bitwise disjunction of named constants to specify the
 * entry mode (cursor and text movement to occur after characters are sent), a
 * disjunction of named constants to specify the display mode (whether the
 * display is on, whether the underscore cursor is displayed, and whether the
 * cursor's location blinks), or one shift command (shift display left/right or
 * shift cursor left/right).
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 * @param command constant(s) that specify the entry mode, display mode, or
 *      shift command
 */
void cowpi_hd44780_send_command(const cowpi_display_module_protocol_t *configuration, uint8_t command);

/**
 * @brief Displays a character at the cursor's location on the HD44780 character
 *      display.
 *
 * The character is an ASCII or "extended-ASCII" character, or one of the eight
 * custom characters created by using `cowpi_hd44780_create_character()`.
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 * @param character the character to be displayed
 */
void cowpi_hd44780_send_character(const cowpi_display_module_protocol_t *configuration, uint8_t character);

/**
 * @brief Creates a custom character for the HD44780 character display.
 *
 * The encoding can be a value between 0 and 7, inclusive. Each element of the
 * pixel vector corresponds to a row of the 5x8 character matrix (thus, only
 * 5 bits of each element are used).
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 * @param encoding the value used to represent the character
 * @param pixel_vector identifies which LCDs are "on" or "off" for the custom
 *      character.
 */
void cowpi_hd44780_create_character(const cowpi_display_module_protocol_t *configuration,
                                    uint8_t encoding, const uint8_t pixel_vector[8]);

/**
 * @brief Clears the display and places the cursor in row 0, column 0.
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 */
void cowpi_hd44780_clear_display(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Places the cursor in row 0, column 0.
 *
 * If the display was shifted, the display is shifted back to its original
 * position. The contents of each character position remain unchanged.
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 */
void cowpi_hd44780_return_home(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Turns the display module's backlight on or off.
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 * @param backlight_on indicates whether the backlight should be on (`true`) or
 *      off (`false`)
 */
void cowpi_hd44780_set_backlight(const cowpi_display_module_protocol_t *configuration, bool backlight_on);

#ifdef __cplusplus
} // extern "C"
#endif


/* Entry Mode Commands */
#define LCDENTRY_CURSORMOVESRIGHT   0x06    //!< Instructs the display module to shift the cursor right after a character is displayed
#define LCDENTRY_CURSORMOVESLEFT    0x04    //!< Instructs the display module to shift the cursor left after a character is displayed
#define LCDENTRY_TEXTSHIFTS         0x05    //!< Instructs the display module to shift the entire display after a character is displayed
#define LCDENTRY_TEXTNOSHIFT        0x04    //!< Instructs the display module not to shift the display after a character is displayed

/* Display On/Off Commands */
#define LCDONOFF_DISPLAYON          0x0C    //!< Instructs the display module to turn on the display
#define LCDONOFF_DISPLAYOFF         0x08    //!< Instructs the display module to turn off the display
#define LCDONOFF_CURSORON           0x0A    //!< Instructs the display module to turn on the underscore cursor
#define LCDONOFF_CURSOROFF          0x08    //!< Instructs the display module to turn off the underscore cursor
#define LCDONOFF_BLINKON            0x09    //!< Instructs the display module to blink the cursor's position
#define LCDONOFF_BLINKOFF           0x08    //!< Instructs the display module not to blink the cursor's position

/* Cursor or Display Shift Commands */
#define LCDSHIFT_DISPLAYLEFT        0x18    //!< Shifts the entire display to the left
#define LCDSHIFT_DISPLAYRIGHT       0x1C    //!< Shifts the entire display to the right
#define LCDSHIFT_CURSORLEFT         0x10    //!< Shifts the cursor to the left
#define LCDSHIFT_CURSORRIGHT        0x14    //!< Shifts the cursor to the right

#endif //COWPI_STDIO_HD44780_H
