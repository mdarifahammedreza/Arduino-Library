/**************************************************************************//**
 *
 * @file hd44780_streams.c
 *
 * Implementations of functions specific to FILE streams for HD44780-based LCD
 * character displays.
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
#include "file_streams_internal.h"
#include "../hd44780/hd44780.h"

int cowpi_lcd_character_put(void *cookie, const char *buffer, int size) {
    stream_data_t *stream_data = (stream_data_t *) cookie;
    uint8_t width = stream_data->display_module.width;
    uint8_t height = stream_data->display_module.height;
    static uint8_t ddram_address = 0;
    static int8_t row = 0;
    static const uint8_t row_starts[] = {0x00, 0x40, 0x14, 0x54};
    static bool scrolled = false;
    int i = 0;
    while (i < size) {
        char c = buffer[i];
        uint8_t row_start = row_starts[row];
        if (ddram_address == row_start) {
            cowpi_hd44780_place_cursor(&stream_data->configuration, ddram_address);
        }
        switch (c) {
            case '\n':
                if (!scrolled) {
                    while (ddram_address < row_start + width) {
                        cowpi_hd44780_send_character(&stream_data->configuration, ' ');
                        ddram_address++;
                    }
                }
                /* FALLTHROUGH */
            case '\v':
                if (!scrolled) {
                    row = INCREMENT_MODULO(row, height);
                    row_start = row_starts[row];
                }
                /* FALLTHROUGH */
            case '\r':
                ddram_address = row_start;
                scrolled = false;
                break;
            case '\f':
                row = 0;
                ddram_address = row_starts[row];
                scrolled = false;
                break;
            case '\t':
                ddram_address++;
                if (ddram_address == row_start + width) {
                    scrolled = true;
                    row = INCREMENT_MODULO(row, height);
                    ddram_address = row_starts[row];
                } else {
                    scrolled = false;
                }
                cowpi_hd44780_place_cursor(&stream_data->configuration, ddram_address);
                break;
            default:    // I'm breaking my rule about using `default` only for error cases
                cowpi_hd44780_send_character(&stream_data->configuration, c);
                ddram_address++;
                if (ddram_address == row_start + width) {
                    scrolled = true;
                    row = INCREMENT_MODULO(row, height);
                    ddram_address = row_starts[row];
                    cowpi_hd44780_place_cursor(&stream_data->configuration, row_start);
                } else {
                    scrolled = false;
                }
        }
        i++;
    }
    return i;
}
