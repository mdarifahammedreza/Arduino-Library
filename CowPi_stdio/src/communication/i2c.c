/**************************************************************************//**
 *
 * @file i2c.c
 *
 * @brief @copybrief i2c.h
 *
 * @copydetails i2c.h
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
#include <stdbool.h>
#include "i2c.h"
#include "../translations/translations.h"

static uint8_t data_pin;
static uint8_t clock_pin;

bool (*cowpi_i2c_initialize)(const cowpi_display_module_protocol_t *configuration) = cowpi_i2c_initialize_hardware;

bool (*cowpi_i2c_transmit)(uint8_t byte) = cowpi_i2c_transmit_hardware;

void (*cowpi_i2c_finalize)(void) = cowpi_i2c_finalize_hardware;


#define WRITE_LOW(pin)  do { cowpi_pin_mode((pin), OUTPUT); } while(0)
#define WRITE_HIGH(pin) do { cowpi_pin_mode((pin), INPUT);  } while(0)

void cowpi_use_i2c_bitbang() {
    cowpi_i2c_initialize = cowpi_i2c_initialize_bitbang;
    cowpi_i2c_transmit = cowpi_i2c_transmit_bitbang;
    cowpi_i2c_finalize = cowpi_i2c_finalize_bitbang;
}

bool cowpi_i2c_initialize_bitbang(const cowpi_display_module_protocol_t *configuration) {
    data_pin = configuration->data_pin;
    clock_pin = configuration->clock_pin;

    // hold bus "free" for 4.7us (t_buf) between stop and start (almost certainly redundant)
    WRITE_HIGH(data_pin);
    WRITE_HIGH(clock_pin);
    delayMicroseconds(5);
    // start bit
    WRITE_LOW(data_pin);
    digitalWrite(data_pin, LOW);    // first time, just to be sure
    delayMicroseconds(5);       // .3us t_icf + 4us t_sth (start hold)
    WRITE_LOW(clock_pin);
    digitalWrite(clock_pin, LOW);   // first time, just to be sure
    // I2C address + /w
    return cowpi_i2c_transmit_bitbang(configuration->i2c_address << 1);
}

bool cowpi_i2c_transmit_bitbang(uint8_t byte) {
    // On a 16MHz ATMega328P, `shiftOut` should give us 77-91MHz clock, might be too fast for I2C in practice.
    // Probably same (but maybe faster) on Arduino Nano Every.
    // Will definitely be too fast on Arduino's ARM-based devices.
    for (int i = 0; i < 8; i++) {
        delayMicroseconds(4);   // 4.7us t_scl, minus the 1us we're going to delay for t_sds
        if ((byte >> (7 - i)) & 0x1) {
            WRITE_HIGH(data_pin);
        } else {
            WRITE_LOW(data_pin);
        }
        delayMicroseconds(2);   // 1us t_icr + 250ns t_sds
        WRITE_HIGH(clock_pin);
        delayMicroseconds(6);   // 1us t_icr + 4us t_sch + 0.3us t_icf + 0ns t_sdh
        WRITE_LOW(clock_pin);
    }
//    pinMode(data_pin, INPUT);
    cowpi_pin_mode(data_pin, INPUT);
    delayMicroseconds(6);
    WRITE_HIGH(clock_pin);
    delayMicroseconds(3);
    uint8_t nack = digitalRead(data_pin);
    delayMicroseconds(3);
    WRITE_LOW(clock_pin);
//    pinMode(data_pin, OUTPUT);
    cowpi_pin_mode(data_pin, OUTPUT);

    WRITE_LOW(data_pin);
    return !nack;
}

void cowpi_i2c_finalize_bitbang(void) {
    // stop bit
    WRITE_LOW(data_pin);        // should already be low from ack bit, but just to be sure
    WRITE_LOW(clock_pin);       // should already be low, but just to be sure
    delayMicroseconds(5);   // 5us t_sch
    WRITE_HIGH(clock_pin);
    delayMicroseconds(5);   // 1us t_icr + 4us t_sps
    WRITE_HIGH(data_pin);
}


#if defined(__AVR_ATmega328P__) || defined (__AVR_ATmega2560__)

bool cowpi_use_i2c_hardware() {
    cowpi_i2c_initialize = cowpi_i2c_initialize_hardware;
    cowpi_i2c_transmit = cowpi_i2c_transmit_hardware;
    cowpi_i2c_finalize = cowpi_i2c_finalize_hardware;
    return true;
}

bool cowpi_i2c_initialize_hardware(const cowpi_display_module_protocol_t *configuration) {
    static bool initialized = false;
    if (!initialized) {
        /* Set SCL Frequency [100kHz] = CPU Clock Frequency [16MHz] / (16 + 2 * TWBR * prescaler [1]) */
        TWBR = 72;
        TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
        /* We won't rely on interrupts since students will work with display before knowing interrupts. */
        /* We're also not going to enable acknowledgements: the basic labs will only use
         * controller-transmitter mode, and without interrupts we can't honor the ACK anyway. */
        /* So we'll just enable TWI */
        TWCR = (1 << TWEN) | (1 << TWEA);
        initialized = true;
    }
    // start bit
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTA);
    while (!(TWCR & (1 << TWINT))) {}
//    if ((TWSR & 0xF8) != 0x08) cowpi_error("Controller did not send I2C start!");
    // I2C address + /w
    TWDR = configuration->i2c_address << 1;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))) {}
//    if ((TWSR & 0xF8) != 0x18) cowpi_error("I2C peripheral did not receive address!");
    return ((TWSR & 0xF8) == 0x18);
}

bool cowpi_i2c_transmit_hardware(uint8_t byte) {
    TWDR = byte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))) {}
//    if ((TWSR & 0xF8)!= 0x28) cowpi_error("I2C peripheral did not receive byte!");
    return ((TWSR & 0xF8) == 0x28);
}

void cowpi_i2c_finalize_hardware(void) {
    // stop bit
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    // need a timed-wait for stop bit to transmit (TWINT does not get set) -- 8 bits / 100000 bps = 80us.
    delayMicroseconds(100);
}

#else

bool cowpi_use_i2c_hardware() {
    cowpi_use_i2c_bitbang();
    return false;
}

bool cowpi_i2c_initialize_hardware(const cowpi_display_module_protocol_t *configuration) {
    return cowpi_i2c_initialize_bitbang(configuration);
}

bool cowpi_i2c_transmit_hardware(uint8_t byte) {
    return cowpi_i2c_transmit_bitbang(byte);
}

void cowpi_i2c_finalize_hardware(void) {
    cowpi_i2c_finalize_bitbang();
}

#endif //architecture


int8_t cowpi_discover_i2c_address(uint8_t i2c_data_pin, uint8_t i2c_clock_pin) {
    int8_t discovered_address = 0;
    int8_t number_of_discovered_addresses = 0;
    cowpi_display_module_protocol_t detail = (cowpi_display_module_protocol_t) {
            .data_pin = i2c_data_pin,
            .clock_pin = i2c_clock_pin,
            .i2c_address = 1
    };
    cowpi_i2c_initialize_bitbang(&detail);  // clear-out an ACK that might be lingering on the line after a reset
    for (int8_t address = 1; address > 0; address++) {
        detail = (cowpi_display_module_protocol_t) {
                .data_pin = COWPI_DEFAULT_I2C_DATA_PIN,
                .clock_pin = COWPI_DEFAULT_I2C_CLOCK_PIN,
                .i2c_address = address
        };
        int ack = cowpi_i2c_initialize_bitbang(&detail);
        cowpi_i2c_finalize_bitbang();
        if (ack) {
            discovered_address = (int8_t) (discovered_address ? -1 : address);
            number_of_discovered_addresses++;
        }
    }
    if (number_of_discovered_addresses == 127) {    // if this happens, then we're likely missing the pull-up resistors
        return 0;
    } else {
        return discovered_address;
    }
}