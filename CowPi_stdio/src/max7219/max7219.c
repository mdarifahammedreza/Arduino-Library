/**************************************************************************//**
 *
 * @file max7219.c
 *
 * @brief @copybrief max7219.h
 *
 * @copydetails max7219.h
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

#include <Arduino.h>
#include <assert.h>
#include "max7219.h"
#include "../communication/communication.h"

void cowpi_setup_max7219(const cowpi_display_module_protocol_t *configuration) {
    assert(configuration->protocol == SPI);
    /* Clear all digit registers */
    for (int i = 1; i <= 8; i++) {
        cowpi_max7219_send(configuration, i, 0);
    }
    /* Take display out of decode mode */
    cowpi_max7219_no_decode(configuration);
    /* Intensity at 7/32 */
    cowpi_max7219_set_intensity(configuration, 3);
    /* Scan all eight digits */
    cowpi_max7219_send(configuration, 0xB, 7);
    /* Take display out of shutdown mode */
    cowpi_max7219_startup(configuration);
    /* Take display out of test mode */
    cowpi_max7219_send(configuration, 0xF, 0);
}

void cowpi_max7219_send(const cowpi_display_module_protocol_t *configuration, uint8_t address, uint8_t data) {
    cowpi_spi_initialize(configuration, COWPI_MSB_FIRST);
    digitalWrite(configuration->select_pin, LOW);
    cowpi_spi_transmit(address);
    cowpi_spi_transmit(data);
    digitalWrite(configuration->select_pin, HIGH);
    cowpi_spi_finalize();
}

void cowpi_max7219_no_decode(const cowpi_display_module_protocol_t *configuration) {
    cowpi_max7219_send(configuration, 0x9, 0);
}

void cowpi_max7219_bcd_decode(const cowpi_display_module_protocol_t *configuration) {
    cowpi_max7219_send(configuration, 0x9, 0xFF);
}

void cowpi_max7219_set_intensity(const cowpi_display_module_protocol_t *configuration, uint8_t intensity) {
    cowpi_max7219_send(configuration, 0xA, intensity & 0xF);
}

void cowpi_max7219_shutdown(const cowpi_display_module_protocol_t *configuration) {
    cowpi_max7219_send(configuration, 0xC, 0);
}

void cowpi_max7219_startup(const cowpi_display_module_protocol_t *configuration) {
    cowpi_max7219_send(configuration, 0xC, 1);
}
