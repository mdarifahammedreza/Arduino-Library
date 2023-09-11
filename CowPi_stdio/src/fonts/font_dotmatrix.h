/**************************************************************************//**
 *
 * @file font_dotmatrix.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Provides a font to display printable ASCII characters on dot-matrix
 *      displays.
 *
 * All characters are guaranteed to occupy a rectangle no greater than 8 rows
 * by 5 columns.
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

#ifndef COWPI_STDIO_FONT_DOTMATRIX_H
#define COWPI_STDIO_FONT_DOTMATRIX_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides the minimum number of columns needed to represent a character
 *      in the dot matrix font.
 *
 * @param   c the character whose width is to be determined
 * @return  the dot matrix width of the character
 */
int8_t cowpi_font_get_dotmatrix_width(char c);

/**
 * @brief Provides the dot matrix representation of an ASCII character with no
 *      blank columns to the right of the character.
 *
 * Each byte of the destination array stores one row of the character's dot
 * matrix representation, with element 0 being the top of the character. Each
 * bit position in an array element represents a column.
 *
 * The `destination` array is assumed to point to a block of memory with 8
 * bytes allocated; that is, it must have enough space to hold the character's
 * dot matrix representation.
 *
 * @sa cowpi_font_ascii_to_5wide_dotmatrix
 * @sa cowpi_font_ascii_to_8wide_dotmatrix
 *
 * @param destination   an empty 8-byte array
 * @param c             the ASCII character whose dot matrix representation is
 *      to be provided
 * @return              `destination`, with the requested dot matrix character
 */
uint8_t *cowpi_font_ascii_to_narrow_dotmatrix(uint8_t *destination, char c);

/**
 * @brief Provides the dot matrix representation of an ASCII character centered
 *      horizontally in the five columns used for a 5x8 dot matrix character
 *      (the five right-most columns of the 8x8 matrix).
 *
 * Each byte of the destination array stores one row of the character's dot
 * matrix representation, with element 0 being the top of the character.
 *
 * The `destination` array is assumed to point to a block of memory with 8
 * bytes allocated; that is, it must have enough space to hold the character's
 * dot matrix representation.
 *
 * @sa cowpi_font_ascii_to_narrow_dotmatrix
 * @sa cowpi_font_ascii_to_8wide_dotmatrix
 *
 * @param destination   an empty 8-byte array
 * @param c             the ASCII character whose dot matrix representation is
 *      to be provided
 * @return              `destination`, with the requested dot matrix character
 */
uint8_t *cowpi_font_ascii_to_5wide_dotmatrix(uint8_t *destination, char c);

/**
 * @brief Provides the dot matrix representation of an ASCII character centered
 *      horizontally in the eight columns of the 8x8 matrix.
 *
 * Each byte of the destination array stores one row of the character's dot
 * matrix representation, with element 0 being the top of the character.
 *
 * The `destination` array is assumed to point to a block of memory with 8
 * bytes allocated; that is, it must have enough space to hold the character's
 * dot matrix representation.
 *
 * @sa cowpi_font_ascii_to_narrow_dotmatrix
 * @sa cowpi_font_ascii_to_5wide_dotmatrix
 *
 * @param destination   an empty 8-byte array
 * @param c             the ASCII character whose dot matrix representation is
 *      to be provided
 * @return              `destination`, with the requested dot matrix character
 */
uint8_t *cowpi_font_ascii_to_8wide_dotmatrix(uint8_t *destination, char c);

/**
 * @brief Transposes an 8x8 dot matrix representation.
 *
 * After the transposition, each element of the `matrix` array represents a
 * column, and each bit position of an array element represents a row.
 *
 * This is an in-place transposition; that is, the original matrix is
 * overwritten.
 *
 * @param matrix    the dot matrix to be transposed
 * @return          `matrix` after the transposition
 */
uint8_t *cowpi_font_transpose_dotmatrix(uint8_t *matrix);

/**
 * @brief Provides a single dot matrix representation of an ASCII character
 *      string suitable for scrolling horizontally.
 *
 * Each byte of the destination array stores one column of the string's dot
 * matrix representation, with element 0 being the leftmost column of the first
 * character.
 *
 * The `destination` array is assumed to point to a block of memory with
 * sufficient space allocated to hold the full string's dot matrix
 * representation; `6 * strlen(s)` is guaranteed to be sufficient.
 *
 * @param destination   an empty array to hold the dot matrix representation
 * @param s             the ASCII string whose dot matrix representation is to
 *      be provided
 * @param kerned        indicates whether to use minimal space between
 *      characters or to use a fixed 5 columns per character
 * @return              the number of columns in the string's dot matrix
 *      representation
 */
int cowpi_font_string_to_horizontal_dotmatrix(uint8_t *destination, const char *s, bool kerned);

/**
 * @brief Provides a single dot matrix representation of an ASCII character
 *      string suitable for scrolling vertically.
 *
 * Each byte of the destination array stores one row of the string's dot matrix
 * representation, with element 0 being the top row of the first character.
 *
 * The `destination` array is assumed to point to a block of memory with
 * sufficient space allocated to hold the full string's dot matrix
 * representation; `9 * strlen(s)` is guaranteed to be sufficient.
 *
 * @param destination   an empty array to hold the dot matrix representation
 * @param s             the ASCII string whose dot matrix representation is to
 *      be provided
 * @param kerned        indicates whether to use minimal space between
 *      characters or to use a fixed 8 rows per character
 * @return              the number of rows in the string's dot matrix
 *      representation
 */
int cowpi_font_string_to_vertical_dotmatrix(uint8_t *destination, const char *s, bool kerned);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_FONT_DOTMATRIX_H
