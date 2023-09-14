/**************************************************************************//**
 *
 * @file max7219.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Utility functions for peripherals driven by MAX7219 such as
 *      8-digit/7-segment display modules and 8x8 LED matrices.
 *
 * The MAX7219 will initially be configured to use all eight digits (or all 8
 * rows in the case of an 8x8 LED matrix), intensity level 3 (7/32 of full
 * brightness), no-decode, not shut-down, and not in test mode.
 *
 * The MAX7219 is simple enough that firmware programs can send data (or
 * commands) to it directly through `cowpi_max7219_send()`; however, convenient
 * abstractions can also be built on top of `cowpi_max7219_send()`.
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

#ifndef COWPI_STDIO_MAX7219_H
#define COWPI_STDIO_MAX7219_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../typedefs.h"


/**
 * @brief Configures the MAX7219 to nominal settings
 * 
 * @param configuration the pins used for SPI
 */
void cowpi_setup_max7219(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Sends a byte of data to be placed in the specified MAX7219 register.
 *
 * The firmware program should only need to send data to addresses 1-8, the
 * digit registers. Most other registers are handled by specific `cowpi_max7219`
 * functions.
 *
 * This is a portable software-only implementation; it does not use SPI
 * hardware, nor does it use memory-mapped I/O of any form. Re-implementing this
 * function (using a different function name) to use SPI hardware is a possible
 * part of a memory-mapped I/O lab assignment.
 *
 * @param configuration the pins used for SPI
 * @param address the register to be updated
 * @param data the byte to place in that register
 */
void cowpi_max7219_send(const cowpi_display_module_protocol_t *configuration, uint8_t address, uint8_t data);

/**
 * @brief Shuts-down the MAX7219 display module.
 *
 * This function places a 0 in the MAX7219's Shutdown register, causing the
 * display to blank. Data in the digit and control registers remain unchanged.
 *
 * @param configuration the pins used for SPI
 *
 * @sa cowpi_max7219_startup
 */
void cowpi_max7219_shutdown(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Takes the MAX7219 display module out of shutdown mode.
 *
 * This function places a 1 in the MAX7219's Shutdown register, causing the
 * display to resume.
 *
 * @param configuration the pins used for SPI
 *
 * @sa cowpi_max7219_shutdown
 */
void cowpi_max7219_startup(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Takes the MAX7219 out of decode mode.
 *
 * This function places 0 in the MAX7219's Decode-Mode register, causing the
 * display to map the data register bits directly to the digits' segments.
 *
 * @param configuration the pins used for SPI
 *
 * @sa cowpi_max_bcd_decode
 */
void cowpi_max7219_no_decode(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Places the MAX7219 in decode mode.
 *
 * This function places 0xFF in the MAX7219's Decode-Mode register, causing the
 * display to interpret the data register bits as BCD values and place the
 * corresponding decimal numeral on the digits.
 *
 * This function should not be called when using an 8x8 LED matrix -- there's
 * no harm in doing so, but decode mode really is meant for 7-segment displays.
 *
 * @param configuration the pins used for SPI
 *
 * @sa cowpi_max_bcd_decode
 */
void cowpi_max7219_bcd_decode(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Sets the MAX7219 display module's brightness level.
 *
 * This function places the argument in the MAX7219's Intensity register. The
 * MAX7219 ignores the upper four bits, so there are 16 levels of brightness.
 *
 * @param configuration the pins used for SPI
 * @param intensity the desired brightness level
 */
void cowpi_max7219_set_intensity(const cowpi_display_module_protocol_t *configuration, uint8_t intensity);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_MAX7219_H
