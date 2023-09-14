/**************************************************************************//**
 *
 * @file file_streams.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Functions to configure file streams for `stdio.h` functions.
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

#ifndef COWPI_STDIO_FILE_STREAMS_H
#define COWPI_STDIO_FILE_STREAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "typedefs.h"

/**
 * @brief Configures the "Serial Monitor" for `printf()` and `scanf()`.
 *
 * Configures `stdout` and `stdin`, allowing programmers to use `printf()` to
 * write to, and `scanf()` to read from, the USB-based serial interface between
 * the microcontroller and the host computer.
 *
 * @param bitrate the serial interface's bit rate
 */
void cowpi_stdio_setup(unsigned long bitrate);

/**
 * Configures a display module for `fprintf()`.
 *
 * @param display_module the display module's details
 * @param configuration the communication protocol's details
 * @return a pointer to a `FILE` stream for the display module, or `NULL` if a
 *      stream could not be created
 */
FILE *cowpi_add_display_module(cowpi_display_module_t display_module, cowpi_display_module_protocol_t configuration);

/**
 * Removes the contents of the display module and places the cursor in the home
 * position.
 *
 * @param filestream a pointer to the `FILE` stream for the display module to
 *      be cleared
 */
void cowpi_clear_display(FILE *filestream);

/**
 * @brief Places a display module in a low-power state.
 *
 * If the display module has a low-power mode that preserves the contents of
 * the display, then this function will place the display module in that mode.
 *
 * @param filestream a pointer to the `FILE` stream for the display module to
 *      be placed in a low-power state
 */
void cowpi_sleep_display(FILE *filestream);

/**
 * @brief Takes a display module out of a low-power state.
 *
 * If the display module has a low-power mode that preserves the contents of
 * the display, then this function will take the display out of that mode,
 * restoring the display's contents.
 *
 * @param filestream a pointer to the `FILE` stream for the display module to
 *      be brought out of a low-power state
 */
void cowpi_wake_display(FILE *filestream);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_FILE_STREAMS_H
