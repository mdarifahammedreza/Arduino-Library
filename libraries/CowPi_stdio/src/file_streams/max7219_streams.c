/**************************************************************************//**
 *
 * @file max7219_streams.c
 *
 * Implementations of functions specific to FILE streams for MAX7219-based
 * 7-segment displays and MAX7219-based LED matrix displays.
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
#include <string.h>
#include "file_streams_internal.h"
#include "../fonts/fonts.h"
#include "../max7219/max7219.h"

static void send_segment_pattern_from_buffer(void *symbol);
static void send_matrix_pattern_from_buffer(void *symbol);

int cowpi_seven_segment_noscroll_put(void *cookie, const char *buffer, int size) {
    stream_data_t *stream_data = (stream_data_t *) cookie;
    static int8_t left_to_right_position = 0;
    static bool escaped = false;
    uint8_t pattern;
    int i = 0;
    while (i < size) {
        if (escaped) {
            pattern = buffer[i];
            if (left_to_right_position < stream_data->display_module.width) {
                cowpi_max7219_send(&stream_data->configuration,
                                   stream_data->display_module.width - left_to_right_position,
                                   pattern);
                left_to_right_position++;
            }
            escaped = false;
        } else {
            char c = buffer[i];
            switch (c) {
                case 0x1B:
                    escaped = true;
                    break;
                case '\a':
                    break;
                case '\n':
                    pattern = cowpi_font_ascii_to_7segment(' ');
                    while (left_to_right_position < stream_data->display_module.width) {
                        cowpi_max7219_send(&stream_data->configuration,
                                           stream_data->display_module.width - left_to_right_position,
                                           pattern);
                        left_to_right_position++;
                    }
                    /* FALLTHROUGH */
                case '\v':  // with only one row, there's not much vertical movement going on
                case '\r':
                case '\f':
                    left_to_right_position = 0;
                    break;
                case '\b':
                    if (left_to_right_position > 0) {
                        left_to_right_position--;
                    }
                    break;
                case 0x7F:
                    if (left_to_right_position > 0) {
                        left_to_right_position--;
                        pattern = cowpi_font_ascii_to_7segment(' ');
                        cowpi_max7219_send(&stream_data->configuration,
                                           stream_data->display_module.width - left_to_right_position,
                                           pattern);
                    }
                    break;
                case '\t':
                    if (left_to_right_position < stream_data->display_module.width) {
                        left_to_right_position++;
                    }
                    break;
                default:    // I'm breaking my rule about using `default` only for error cases
                    pattern = cowpi_font_ascii_to_7segment(c);
                    if (left_to_right_position < stream_data->display_module.width) {
                        cowpi_max7219_send(&stream_data->configuration,
                                           stream_data->display_module.width - left_to_right_position,
                                           pattern);
                        left_to_right_position++;
                    }
            }
        }
        i++;
    }
    return i;
}

int cowpi_seven_segment_scrolling_put(void *cookie, const char *buffer, int size) {
    stream_data_t *stream_data = (stream_data_t *) cookie;
    static bool escaped = false;
    uint8_t pattern;
    int i = 0;
    while (i < size) {
        if (escaped) {
            pattern = buffer[i];
            add_symbol_to_buffer((symbol_t) {
                    .callback = send_segment_pattern_from_buffer,
                    .stream_data = stream_data,
                    .symbol = pattern,
                    .symbol_duration = stream_data->ms_per_signal / SYMBOL_DURATION_SCALING_FACTOR
            });
            escaped = false;
        } else {
            char c = buffer[i];
            switch (c) {
                case 0x1B:
                    escaped = true;
                    break;
                case '\a':
                case '\b':
                case 0x7F:
                    break;
                case '\v':
                case '\r':
                case '\f':
                case '\n':
                    pattern = cowpi_font_ascii_to_7segment(' ');
                    uint8_t width = stream_data->display_module.width;
                    for (int j = 0; j < width; j++) {
                        add_symbol_to_buffer((symbol_t) {
                                .callback = send_segment_pattern_from_buffer,
                                .stream_data = stream_data,
                                .symbol = pattern,
                                .symbol_duration = stream_data->ms_per_signal / SYMBOL_DURATION_SCALING_FACTOR
                        });
                    }
                    break;
                case '\t':
                    pattern = cowpi_font_ascii_to_7segment(' ');
                    for (int j = 0; j < 4; j++) {
                        add_symbol_to_buffer((symbol_t) {
                                .callback = send_segment_pattern_from_buffer,
                                .stream_data = stream_data,
                                .symbol = pattern,
                                .symbol_duration = stream_data->ms_per_signal / SYMBOL_DURATION_SCALING_FACTOR
                        });
                    }
                    break;
                default:    // I'm breaking my rule about using `default` only for error cases
                    pattern = cowpi_font_ascii_to_7segment(c);
                    add_symbol_to_buffer((symbol_t) {
                            .callback = send_segment_pattern_from_buffer,
                            .stream_data = stream_data,
                            .symbol = pattern,
                            .symbol_duration = stream_data->ms_per_signal / SYMBOL_DURATION_SCALING_FACTOR
                    });
            }
        }
        i++;
    }
    return i;
}

int cowpi_led_matrix_scrolling_put(void *cookie, const char *buffer, int size) {
    stream_data_t *stream_data = (stream_data_t *) cookie;
    static bool escaped = false;
    uint8_t character[16];
    int8_t character_width = 0;
    int i = 0;
    while (i < size) {
        if (escaped) {
            add_symbol_to_buffer((symbol_t) {
                    .callback = send_matrix_pattern_from_buffer,
                    .stream_data = stream_data,
                    .symbol = buffer[i],
                    .symbol_duration = stream_data->ms_per_signal / SYMBOL_DURATION_SCALING_FACTOR
            });
            escaped = false;
        } else {
            char c = buffer[i];
            switch (c) {
                case 0x1B:
                    escaped = true;
                    break;
                case '\a':
                case '\b':
                case 0x7F:
                    break;
                case '\v':
                case '\r':
                case '\f':
                case '\n':
                    memset(character, 0, 16);
                    character_width = 2*8;
                    break;
                case '\t':
                    memset(character, 0, 16);
                    character_width = 10;
                    break;
                default:    // I'm breaking my rule about using `default` only for error cases
                    cowpi_font_ascii_to_5wide_dotmatrix(character, c);
                    cowpi_font_transpose_dotmatrix(character);
                    character_width = cowpi_font_get_dotmatrix_width(c);
            }
            if (!escaped) {
                int character_offset = character_width > 8 ? 0 : 3 + (5 - character_width) / 2;
                for (int j = 0; j < character_width; j++) {
                    add_symbol_to_buffer((symbol_t) {
                            .callback = send_matrix_pattern_from_buffer,
                            .stream_data = stream_data,
                            .symbol = character[j + character_offset],
                            .symbol_duration = stream_data->ms_per_signal / SYMBOL_DURATION_SCALING_FACTOR
                    });
                }
                // add intercharacter space
                add_symbol_to_buffer((symbol_t) {
                        .callback = send_matrix_pattern_from_buffer,
                        .stream_data = stream_data,
                        .symbol = 0x00,
                        .symbol_duration = stream_data->ms_per_signal / SYMBOL_DURATION_SCALING_FACTOR
                });
            }
        }
        i++;
    }
    return i;
}


static void send_segment_pattern_from_buffer(void *symbol) {
    static uint8_t display_patterns[8] = {0};   // for now, use 8 -- will have to put an upper limit on it later
    symbol_t *symbol_data = (symbol_t *) symbol;
    cowpi_display_module_protocol_t *configuration = &symbol_data->stream_data->configuration;
    uint8_t width = symbol_data->stream_data->display_module.width;
    for (int i = width - 1; i >= 0; i--) {
        if (i > 0) {
            display_patterns[i] = display_patterns[i - 1];
        } else {
            display_patterns[i] = symbol_data->symbol;
        }
        cowpi_max7219_send(configuration, i + 1, display_patterns[i]);
    }
}

static void send_matrix_pattern_from_buffer(void *symbol) {
    static uint8_t incoming_display_patterns[8] = {0};   // for now, use 8 -- will have to put an upper limit on it later
    static uint8_t outgoing_display_patterns[8] = {0};
    symbol_t *symbol_data = (symbol_t *) symbol;
    enum orientations display_orientation = symbol_data->stream_data->display_module.display_orientation;
    enum flips character_flip = symbol_data->stream_data->display_module.character_flip;
    uint8_t width = symbol_data->stream_data->display_module.width;
    cowpi_display_module_protocol_t *configuration = &symbol_data->stream_data->configuration;
    // first, update what the sender thinks is sent to the display
    uint8_t pattern = character_flip ? symbol_data->symbol : reverse_byte(symbol_data->symbol);
    for (int i = width - 1; i >= 0; i--) {
        if (i > 0) {
            incoming_display_patterns[i] = incoming_display_patterns[i - 1];
        } else {
            incoming_display_patterns[i] = pattern;
        }
    }
    // then, send it to the display
    memcpy(outgoing_display_patterns, incoming_display_patterns, 8);
    switch (display_orientation) {
        case SOUTH:
            cowpi_font_transpose_dotmatrix(outgoing_display_patterns);
            /* FALLTHROUGH */
        case EAST:
            for (int i = 0; i < width; i++) {
                cowpi_max7219_send(configuration, i+1, outgoing_display_patterns[i]);
            }
            break;
        case NORTH:
            cowpi_font_transpose_dotmatrix(outgoing_display_patterns);
            /* FALLTHROUGH */
        case WEST:
            for (int i = 1; i <= width; i++) {
                cowpi_max7219_send(configuration, i, reverse_byte(outgoing_display_patterns[width - i]));
            }
            break;
    }
}
