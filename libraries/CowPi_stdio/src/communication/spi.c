/**************************************************************************//**
 *
 * @file spi.c
 *
 * @brief @copybrief spi.h
 *
 * @copydetails spi.h
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
#include "spi.h"


static uint8_t data_pin;
static uint8_t clock_pin;
static bit_order_t data_order;

void (*cowpi_spi_initialize)(const cowpi_display_module_protocol_t *configuration,
                             bit_order_t bit_order) = cowpi_spi_initialize_bitbang;

void (*cowpi_spi_transmit)(uint8_t byte) = cowpi_spi_transmit_bitbang;

void (*cowpi_spi_finalize)(void) = cowpi_spi_finalize_bitbang;


void cowpi_use_spi_bitbang() {
    cowpi_spi_initialize = cowpi_spi_initialize_bitbang;
    cowpi_spi_transmit = cowpi_spi_transmit_bitbang;
    cowpi_spi_finalize = cowpi_spi_finalize_bitbang;
}

void cowpi_spi_initialize_bitbang(const cowpi_display_module_protocol_t *configuration, bit_order_t bit_order) {
    data_pin = configuration->data_pin;
    clock_pin = configuration->clock_pin;
    data_order = bit_order;
}

void cowpi_spi_transmit_bitbang(uint8_t byte) {
    shiftOut(data_pin, clock_pin, data_order == COWPI_MSB_FIRST ? MSBFIRST : LSBFIRST, byte);
}

void cowpi_spi_finalize_bitbang(void) { ; }


#if defined(__AVR_ATmega328P__) || defined (__AVR_ATmega2560__)

bool cowpi_use_spi_hardware() {
    cowpi_spi_initialize = cowpi_spi_initialize_hardware;
    cowpi_spi_transmit = cowpi_spi_transmit_hardware;
    cowpi_spi_finalize = cowpi_spi_finalize_hardware;
    return true;
}

void cowpi_spi_initialize_hardware(__attribute__((unused)) const cowpi_display_module_protocol_t *configuration,
                                   bit_order_t bit_order) {
    uint8_t bit_order_bit = ((bit_order == COWPI_LSB_FIRST) << DORD);
    /* Enable SPI, data order, Controller, set clock rate fck/16 [1MHz] */
    SPCR = (1 << SPE) | bit_order_bit | (1 << MSTR) | (1 << SPR0);
}

void cowpi_spi_transmit_hardware(uint8_t byte) {
    SPDR = byte;
    while (!(SPSR & 0x80)) {}
}

void cowpi_spi_finalize_hardware(void) {
    SPCR = 0;
}

#else

bool cowpi_use_spi_hardware() {
    cowpi_use_spi_bitbang();
    return false;
}

void cowpi_spi_initialize_hardware(const cowpi_display_module_protocol_t *configuration, bit_order_t bit_order) {
    cowpi_spi_initialize_bitbang(configuration, bit_order);
}

void cowpi_spi_transmit_hardware(uint8_t byte) {
    cowpi_spi_transmit_bitbang(byte);
}

void cowpi_spi_finalize_hardware(void) {
    cowpi_spi_finalize_bitbang();
}

#endif //architecture
