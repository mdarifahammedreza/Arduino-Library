/**************************************************************************//**
 *
 * @file font_7segment.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Provides a font to display approximations of printable ASCII
 *      characters on 7-segment displays.
 *
 * The mapping of bits to segment lines is:
 * ```
 *  --A--
 * |     |
 * F     B
 * |     |              bit      7 6 5 4 3 2 1 0
 *  --G--           segment     DP A B C D E F G
 * |     |
 * E     C
 * |     |
 *  --D--   (DP)
 * ```
 * If your display follows the same mapping, you can use the functions' outputs
 * directly. If the endianness of segments A-G are the same but your decimal
 * point is bit 0, then you can left-shift the functions' outputs by 1 and only
 * certain punctuation characters will be improperly displayed. If the mapping
 * is any further off, then you will likely not be able to use these functions.
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

#ifndef COWPI_STDIO_FONT_7SEGMENT_H
#define COWPI_STDIO_FONT_7SEGMENT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides the 7-segment bit vector for an ASCII character or for a
 *      hexadecimal numeral.
 *
 * For hexadecimal numerals 0x0-0xF, returns a bit vector to display that
 * numeral on a 7-segment display. For the printable ASCII characters 0x20-0x7E,
 * returns a bit vector to display an approximation of that character on a
 * 7-segment display. Values in the range 0x10-0x1F, and 0x7F (DEL) return 0xFF,
 * which will illuminate all segments.
 *
 * The bit vector is guaranteed to work for a 7-segment display whose segments
 * are mapped to bits in the manner specified in the MAX7219/MAX7221 datasheet
 * (MSB DP-A-B-C-D-E-F-G LSB).
 *
 * @param c the ASCII character whose 7-segment bit vector is to be provided
 * @return  a 7-segment bit vector
 */
uint8_t cowpi_font_ascii_to_7segment(char c);

/**
 * @brief Provides an array of 7-segment bit vectors for an ASCII character
 *      string.
 *
 * The source string, `s`, is assumed to be NUL-terminated.
 *
 * The `destination` array is assumed to point to a block of memory with
 * `strlen(s)` bytes allocated; that is, it must have enough space to hold
 * the array of bit vectors.
 *
 * @note The array of bit vectors will *not* be NUL-terminated.
 *
 * @param destination   an empty array of sufficient size to hold the bit
 *      vectors
 * @param s             the string to be converted into 7-segment bit vectors
 * @return              `destination`, with the requested bit vectors
 */
uint8_t *cowpi_font_string_to_7segment(uint8_t *destination, const char *s);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_FONT_7SEGMENT_H
