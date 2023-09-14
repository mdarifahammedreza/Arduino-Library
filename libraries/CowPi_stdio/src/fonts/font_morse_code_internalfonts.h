/**************************************************************************//**
 *
 * @file font_morse_code_internalfonts.h
 *
 * @author Christopher A. Bohn
 *
 * @brief The specific static constants for the font promised by
 *      font_morse_code.h
 *
 * This file should be `#include`d *only* by fonts.c
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

#ifdef COWPI_STDIO_FONTS_INTERNAL

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "font_morse_code.h"

#ifdef __AVR__

#include <avr/pgmspace.h>

#else
#define PROGMEM
#endif //__AVR__

static const uint8_t cowpi_font_morse_skip[]                        PROGMEM = {CHARACTER_SPACE, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_error[]                       PROGMEM = {CHARACTER_SPACE, DIT, DIT, DIT, DIT, DIT, DIT, DIT, DIT, END_OF_CHARACTER};

static const uint8_t cowpi_font_morse_newparagraph[]      /* BT */  PROGMEM = {WORD_SPACE, DAH, DIT, DIT, DIT, DAH, WORD_SPACE, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_nextline[]          /* AA */  PROGMEM = {WORD_SPACE, DIT, DAH, DIT, DAH, WORD_SPACE, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_break[]             /* BK */  PROGMEM = {WORD_SPACE, DAH, DIT, DIT, DIT, DAH, DIT, DAH, WORD_SPACE, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_start_of_message[]  /* KA */  PROGMEM = {WORD_SPACE, DAH, DIT, DAH, DIT, DAH, WORD_SPACE, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_end_of_message[]    /* AR */  PROGMEM = {WORD_SPACE, DIT, DAH, DIT, DAH, DIT, WORD_SPACE, END_OF_CHARACTER};

static const uint8_t cowpi_font_morse_space[]                       PROGMEM = {WORD_SPACE, END_OF_CHARACTER};

static const uint8_t cowpi_font_morse_bang[]                        PROGMEM = {DAH, DIT, DAH, DIT, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_doublequote[]                 PROGMEM = {DIT, DAH, DIT, DIT, DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_octothorpe[]                  PROGMEM = {END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_dollar[]                      PROGMEM = {END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_percent[]                     PROGMEM = {DAH, DAH, DAH, DAH, DAH, SYMBOL_SPACE,
                                                                               DAH, DIT, DIT, DAH, DIT, SYMBOL_SPACE,
                                                                               DAH, DAH, DAH, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_ampersand[]         /* AS */  PROGMEM = {DIT, DAH, DIT, DIT, DIT, END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_apostrophe[]                  PROGMEM = {END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_leftparen[]                   PROGMEM = {DAH, DIT, DAH, DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_rightparen[]                  PROGMEM = {DAH, DIT, DAH, DAH, DIT, DAH, END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_asterisk[]                    PROGMEM = {END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_plus[]              /* AR */  PROGMEM = {DIT, DAH, DIT, DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_comma[]                       PROGMEM = {DAH, DAH, DIT, DIT, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_dash[]                        PROGMEM = {DAH, DIT, DIT, DIT, DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_period[]                      PROGMEM = {DIT, DAH, DIT, DAH, DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_slash[]                       PROGMEM = {DAH, DIT, DIT, DAH, DIT, END_OF_CHARACTER};

static const uint8_t cowpi_font_morse_zero[]                        PROGMEM = {DAH, DAH, DAH, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_one[]                         PROGMEM = {DIT, DAH, DAH, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_two[]                         PROGMEM = {DIT, DIT, DAH, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_three[]                       PROGMEM = {DIT, DIT, DIT, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_four[]                        PROGMEM = {DIT, DIT, DIT, DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_five[]                        PROGMEM = {DIT, DIT, DIT, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_six[]                         PROGMEM = {DAH, DIT, DIT, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_seven[]                       PROGMEM = {DAH, DAH, DIT, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_eight[]                       PROGMEM = {DAH, DAH, DAH, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_nine[]                        PROGMEM = {DAH, DAH, DAH, DAH, DIT, END_OF_CHARACTER};

static const uint8_t cowpi_font_morse_colon[]                       PROGMEM = {DAH, DAH, DAH, DIT, DIT, DIT, END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_semicolon[]                   PROGMEM = {END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_lessthan[]                    PROGMEM = {END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_equals[]            /* BT */  PROGMEM = {DAH, DIT, DIT, DIT, DAH, END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_greaterthan[]                 PROGMEM = {END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_question[]                    PROGMEM = {DIT, DIT, DAH, DAH, DIT, DIT};
static const uint8_t cowpi_font_morse_at[]                          PROGMEM = {DIT, DAH, DAH, DIT, DAH, DIT, END_OF_CHARACTER};

static const uint8_t cowpi_font_morse_A[]                           PROGMEM = {DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_B[]                           PROGMEM = {DAH, DIT, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_C[]                           PROGMEM = {DAH, DIT, DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_D[]                           PROGMEM = {DAH, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_E[]                           PROGMEM = {DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_F[]                           PROGMEM = {DIT, DIT, DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_G[]                           PROGMEM = {DAH, DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_H[]                           PROGMEM = {DIT, DIT, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_I[]                           PROGMEM = {DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_J[]                           PROGMEM = {DIT, DAH, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_K[]                           PROGMEM = {DAH, DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_L[]                           PROGMEM = {DIT, DAH, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_M[]                           PROGMEM = {DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_N[]                           PROGMEM = {DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_O[]                           PROGMEM = {DAH, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_P[]                           PROGMEM = {DIT, DAH, DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_Q[]                           PROGMEM = {DAH, DAH, DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_R[]                           PROGMEM = {DIT, DAH, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_S[]                           PROGMEM = {DIT, DIT, DIT, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_T[]                           PROGMEM = {DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_U[]                           PROGMEM = {DIT, DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_V[]                           PROGMEM = {DIT, DIT, DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_W[]                           PROGMEM = {DIT, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_X[]                           PROGMEM = {DAH, DIT, DIT, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_Y[]                           PROGMEM = {DAH, DIT, DAH, DAH, END_OF_CHARACTER};
static const uint8_t cowpi_font_morse_Z[]                           PROGMEM = {DAH, DAH, DIT, DIT, END_OF_CHARACTER};

//static const uint8_t cowpi_font_morse_backslash[]                   PROGMEM = {END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_carat[]                       PROGMEM = {END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_underscore[]                  PROGMEM = {END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_backtick[]                    PROGMEM = {END_OF_CHARACTER};

//static const uint8_t cowpi_font_morse_leftbrace[]                   PROGMEM = {END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_pipe[]                        PROGMEM = {END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_rightbrace[]                  PROGMEM = {END_OF_CHARACTER};
//static const uint8_t cowpi_font_morse_tilde[]                       PROGMEM = {END_OF_CHARACTER};


#define MAXIMUM_MORSE_CODE_LENGTH (10)  /* except for percent sign */
#define PERCENT_MORSE_CODE_LENGTH (18)


static const uint8_t *const cowpi_font_morse_code[] PROGMEM = {
        cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_start_of_message,
        cowpi_font_morse_error,         cowpi_font_morse_space,         cowpi_font_morse_newparagraph,  cowpi_font_morse_nextline,      cowpi_font_morse_end_of_message,cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,
        cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,
        cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_skip,
        cowpi_font_morse_space,         cowpi_font_morse_bang,          cowpi_font_morse_doublequote,   cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_percent,       cowpi_font_morse_ampersand,     cowpi_font_morse_skip,
        cowpi_font_morse_leftparen,     cowpi_font_morse_rightparen,    cowpi_font_morse_skip,          cowpi_font_morse_plus,          cowpi_font_morse_comma,         cowpi_font_morse_dash,          cowpi_font_morse_period,        cowpi_font_morse_slash,
        cowpi_font_morse_zero,          cowpi_font_morse_one,           cowpi_font_morse_two,           cowpi_font_morse_three,         cowpi_font_morse_four,          cowpi_font_morse_five,          cowpi_font_morse_six,           cowpi_font_morse_seven,
        cowpi_font_morse_eight,         cowpi_font_morse_nine,          cowpi_font_morse_colon,         cowpi_font_morse_skip,          cowpi_font_morse_skip,          cowpi_font_morse_equals,        cowpi_font_morse_skip,          cowpi_font_morse_question,
        cowpi_font_morse_at,            cowpi_font_morse_A,             cowpi_font_morse_B,             cowpi_font_morse_C,             cowpi_font_morse_D,             cowpi_font_morse_E,             cowpi_font_morse_F,             cowpi_font_morse_G,
        cowpi_font_morse_H,             cowpi_font_morse_I,             cowpi_font_morse_J,             cowpi_font_morse_K,             cowpi_font_morse_L,             cowpi_font_morse_M,             cowpi_font_morse_N,             cowpi_font_morse_O,
        cowpi_font_morse_P,             cowpi_font_morse_Q,             cowpi_font_morse_R,             cowpi_font_morse_S,             cowpi_font_morse_T,             cowpi_font_morse_U,             cowpi_font_morse_V,             cowpi_font_morse_W,
        cowpi_font_morse_X,             cowpi_font_morse_Y,             cowpi_font_morse_Z,             cowpi_font_morse_leftparen,     cowpi_font_morse_skip,          cowpi_font_morse_rightparen,    cowpi_font_morse_skip,          cowpi_font_morse_skip,
        cowpi_font_morse_skip,          cowpi_font_morse_A,             cowpi_font_morse_B,             cowpi_font_morse_C,             cowpi_font_morse_D,             cowpi_font_morse_E,             cowpi_font_morse_F,             cowpi_font_morse_G,
        cowpi_font_morse_H,             cowpi_font_morse_I,             cowpi_font_morse_J,             cowpi_font_morse_K,             cowpi_font_morse_L,             cowpi_font_morse_M,             cowpi_font_morse_N,             cowpi_font_morse_O,
        cowpi_font_morse_P,             cowpi_font_morse_Q,             cowpi_font_morse_R,             cowpi_font_morse_S,             cowpi_font_morse_T,             cowpi_font_morse_U,             cowpi_font_morse_V,             cowpi_font_morse_W,
        cowpi_font_morse_X,             cowpi_font_morse_Y,             cowpi_font_morse_Z,             cowpi_font_morse_leftparen,     cowpi_font_morse_skip,          cowpi_font_morse_rightparen,    cowpi_font_morse_skip,          cowpi_font_morse_error
};

#ifndef __AVR__
#undef PROGMEM
#endif //__AVR__

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_FONTS_INTERNAL
