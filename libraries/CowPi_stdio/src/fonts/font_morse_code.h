/**************************************************************************//**
 *
 * @file font_morse_code.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Provides a "font" to drive the dits and dahs of a Morse Code
 *      illuminator or sounder
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

#ifndef COWPI_STDIO_FONT_MORSE_CODE_H
#define COWPI_STDIO_FONT_MORSE_CODE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Symbols used in Morse Code.
 */
enum {
    END_OF_CHARACTER = 0,
    SYMBOL_SPACE,       // 1 unit
    CHARACTER_SPACE,    // 3 units
    WORD_SPACE,         // 7 units
    DIT,                // 1 unit
    DAH                 // 3 units
};

/**
 * @brief Provides the series of DITs and DAHs to represent an ASCII
 *      character in Morse Code.
 *
 * Each byte of the destination array stores a DIT, a DAH, a SYMBOL_SPACE
 * (the pause between dits/dahs), a CHARACTER_SPACE (the pause between letters),
 * a WORD_SPACE (the pause between words), or an END_OF_CHARACTER (a
 * terminating NUL).
 *
 * In most cases, a 9-byte `destination` array will be sufficient; however,
 * if `c` is a percent sign (`'\%'`) then 18 bytes will be required for the
 * `destination` array.
 *
 * @param destination   an empty 8-byte array
 * @param c             the ASCII character whose Morse Code representation is
 *      to be provided
 * @return              `destination`, with the requested series of Morse
 *      symbols
 */
uint8_t *cowpi_font_ascii_to_morse_code(uint8_t *destination, char c);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_FONT_MORSE_CODE_H
