/**************************************************************************//**
 *
 * @file fonts.c
 *
 * @brief @copybrief fonts.h
 *
 * @copydetails fonts.h
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

#define COWPI_STDIO_FONTS_INTERNAL

#include <stdbool.h>
#include <string.h>
#include "font_7segment.h"
#include "font_7segment_internalfonts.h"
#include "font_dotmatrix.h"
#include "font_dotmatrix_internalfonts.h"
#include "font_morse_code.h"
#include "font_morse_code_internalfonts.h"

#ifdef __AVR__

#include <avr/pgmspace.h>

#endif //__AVR__

/* 7-Segment Functions */

uint8_t cowpi_font_ascii_to_7segment(char c) {
#ifdef __AVR__
    return pgm_read_byte(cowpi_font_7segment + c);
#else
    return cowpi_font_7segment[(int) c];
#endif //__AVR__
}

uint8_t *cowpi_font_string_to_7segment(uint8_t *destination, const char *s) {
    uint8_t *character_destination = destination;
    while (*s) {
        *character_destination++ = cowpi_font_ascii_to_7segment(*s++);
    }
    return destination;
}

/* Dot Matrix Functions */

int8_t cowpi_font_get_dotmatrix_width(char c) {
#ifdef __AVR__
    return pgm_read_byte(cowpi_font_dotmatrix_widths + c);
#else
    return cowpi_font_dotmatrix_widths[(int) c];
#endif //__AVR__
}

uint8_t *cowpi_font_ascii_to_narrow_dotmatrix(uint8_t *destination, char c) {
#ifdef __AVR__
    memcpy_P(destination, (const uint8_t *) pgm_read_word(cowpi_font_dotmatrix + c), 8);
#else
    memcpy(destination, cowpi_font_dotmatrix[(int) c], 8);
#endif //__AVR__
    return destination;
}

uint8_t *cowpi_font_ascii_to_5wide_dotmatrix(uint8_t *destination, char c) {
    destination = cowpi_font_ascii_to_narrow_dotmatrix(destination, c);
    for (int i = 0; i < 8; i++) {
        destination[i] <<= (5 - cowpi_font_get_dotmatrix_width(c) + 1) / 2;
    }
    return destination;
}

uint8_t *cowpi_font_ascii_to_8wide_dotmatrix(uint8_t *destination, char c) {
    destination = cowpi_font_ascii_to_5wide_dotmatrix(destination, c);
    for (int i = 0; i < 8; i++) {
        destination[i] <<= 1;
    }
    return destination;
}

uint8_t *cowpi_font_transpose_dotmatrix(uint8_t *matrix) {
    uint8_t new_matrix[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    for (int i = 7; i >= 0; i--) {
        uint8_t original_row = matrix[i];
        for (int j = 7; j >= 0; j--) {
            new_matrix[j] |= original_row & 0x1;
            original_row >>= 1;
            if (i > 0) {
                new_matrix[j] <<= 1;
            }
        }
    }
    memcpy(matrix, new_matrix, 8);
    return matrix;
}

int cowpi_font_string_to_horizontal_dotmatrix(uint8_t *destination, const char *s, bool kerned) {
    uint8_t letter[8];
    int length = 0;
    while (*s) {
        if (kerned) {
            if (*s == ' ') {
                char last_character = *(s - 1);
                destination[length++] = 0x00;
                destination[length++] = 0x00;
                if (last_character == '.' || last_character == '!' || last_character == '?' || last_character == ':') {
                    destination[length++] = 0x00;
                    destination[length++] = 0x00;
                }
            } else {
                cowpi_font_transpose_dotmatrix(cowpi_font_ascii_to_narrow_dotmatrix(letter, *s));
                for (int i = 0; i < 8; i++) {
                    if (letter[i]) {
                        destination[length++] = letter[i];
                    }
                }
            }
        } else {
            cowpi_font_transpose_dotmatrix(cowpi_font_ascii_to_5wide_dotmatrix(letter, *s));
            for (int i = 3; i < 8; i++) {
                destination[length++] = letter[i];
            }
        }
        s++;
        destination[length++] = 0x00;
    }
    return length;
}

int cowpi_font_string_to_vertical_dotmatrix(uint8_t *destination, const char *s, bool kerned) {
    uint8_t letter[8];
    int length = 0;
    while (*s) {
        cowpi_font_ascii_to_8wide_dotmatrix(letter, *s);
        if (kerned && (*s == ' ')) {
            char last_character = *(s - 1);
            destination[length++] = 0x00;
            destination[length++] = 0x00;
            destination[length++] = 0x00;
            if (last_character == '.' || last_character == '!' || last_character == '?' || last_character == ':') {
                destination[length++] = 0x00;
                destination[length++] = 0x00;
                destination[length++] = 0x00;
            }
        } else {
            for (int i = 0; i < 8; i++) {
                if (!kerned || letter[i] || ((i > 0) && (i < 7) && letter[i - 1] && letter[i + 1])) {
                    destination[length++] = letter[i];
                }
            }
        }
        s++;
        destination[length++] = 0x00;
    }
    return length;
}

/* Morse Code Functions */

uint8_t *cowpi_font_ascii_to_morse_code(uint8_t *destination, char c) {
#ifdef __AVR__
    memcpy_P(destination, (const uint8_t *) pgm_read_word(cowpi_font_morse_code + c),
             c == '%' ? PERCENT_MORSE_CODE_LENGTH : MAXIMUM_MORSE_CODE_LENGTH);
#else
    memcpy(destination, cowpi_font_morse_code[(int) c],
           c == '%' ? PERCENT_MORSE_CODE_LENGTH : MAXIMUM_MORSE_CODE_LENGTH);
#endif //__AVR__
    return destination;
}
