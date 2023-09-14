/**************************************************************************//**
 *
 * @file hd44780.c
 *
 * @brief @copybrief hd44780.h
 *
 * @copydetails hd44780.h
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
#include "hd44780.h"
#include "../communication/communication.h"


#define ENTRY_MODE_MARKER 0x4
static uint8_t last_entry_mode = ENTRY_MODE_MARKER;
static bool is_backlit = false;


cowpi_hd44780_send_halfbyte_t cowpi_hd44780_send_halfbyte = NULL;

static void cowpi_hd44780_set_4bit_mode(const cowpi_display_module_protocol_t *configuration);

static void cowpi_hd44780_send_halfbyte_spi(const cowpi_display_module_protocol_t *configuration,
                                            uint8_t halfbyte, bool is_command);

static void cowpi_hd44780_send_halfbyte_i2c(const cowpi_display_module_protocol_t *configuration,
                                            uint8_t halfbyte, bool is_command);


void cowpi_setup_hd44780(const cowpi_display_module_protocol_t *configuration) {
    assert(configuration->protocol == SPI || configuration->protocol == I2C);
    /* HD44780U datasheet says LCD needs 40ms after Vcc=2.7V, or 15ms after Vcc=4.5V */
    delayMicroseconds(12500);   // Don't want to use delay(50) just in case interrupts are disabled.
    delayMicroseconds(12500);   // Don't want to use delayMicroseconds(50000) because that's 3x longer than
    delayMicroseconds(12500);   // delayMicroseconds is safe for. Note that 16383 == 2**14 - 1 -- this suggests
    delayMicroseconds(12500);   // that while there will be some drift, the real problem is integer overflow
    /* Place in 4-bit mode because 74HC595 is an 8-bit shift register, and we need RS & EN bits, too
     * (likewise, the 8-bit I2C serial adapters)*/
    cowpi_hd44780_set_4bit_mode(configuration);
    /* 4-bit mode, 2 line display, 5x8 dot matrix
     * (use this configuration even for 20x4 display modules) */
    cowpi_hd44780_send_command(configuration, 0x28);
    /* with each character: increment location, do not shift display (0x06) */
    cowpi_hd44780_send_command(configuration, LCDENTRY_CURSORMOVESRIGHT | LCDENTRY_TEXTNOSHIFT);
    /* display on, cursor off, no blink (0x0C) */
    cowpi_hd44780_send_command(configuration, LCDONOFF_DISPLAYON | LCDONOFF_CURSOROFF | LCDONOFF_BLINKOFF);
    /* clear display */
    cowpi_hd44780_clear_display(configuration);
}

void cowpi_hd44780_set_send_halfbyte_function(
        void (*send_halfbyte_function)(const cowpi_display_module_protocol_t *configuration,
                                       uint8_t halfbyte, bool is_command)) {
    cowpi_hd44780_send_halfbyte = send_halfbyte_function;
}

void cowpi_hd44780_place_character(const cowpi_display_module_protocol_t *configuration,
                                   uint8_t address, uint8_t character) {
    cowpi_hd44780_place_cursor(configuration, address);
    cowpi_hd44780_send_character(configuration, character);
}

void cowpi_hd44780_place_cursor(const cowpi_display_module_protocol_t *configuration, uint8_t address) {
    cowpi_hd44780_send_command(configuration, address | 0x80);
}

void cowpi_hd44780_send_command(const cowpi_display_module_protocol_t *configuration, uint8_t command) {
    cowpi_hd44780_send_halfbyte(configuration, (command >> 4) & 0xF, true);
    cowpi_hd44780_send_halfbyte(configuration, command & 0xF, true);
    if (command & ENTRY_MODE_MARKER) {
        last_entry_mode = command;
    }
    delayMicroseconds(
            50);      // most commands require 37us-38us according to various datasheets; the exceptions have their own functions
}

void cowpi_hd44780_send_character(const cowpi_display_module_protocol_t *configuration, uint8_t character) {
    cowpi_hd44780_send_halfbyte(configuration, (character >> 4) & 0xF, false);
    cowpi_hd44780_send_halfbyte(configuration, character & 0xF, false);
    delayMicroseconds(
            50);      // HD44780U datasheet says 41us (37+4) needed until character is updated and ddram address is updated; SPLC780D simply says 38us
}

void cowpi_hd44780_create_character(const cowpi_display_module_protocol_t *configuration,
                                    uint8_t encoding, const uint8_t pixel_vector[8]) {
    uint8_t cgram_start = (encoding & 0x7) << 3;
    for (int row = 0; row < 8; row++) {
        cowpi_hd44780_send_command(configuration, (cgram_start + row) | 0x40);
        cowpi_hd44780_send_character(configuration, pixel_vector[row]);
    }
}

void cowpi_hd44780_clear_display(const cowpi_display_module_protocol_t *configuration) {
    cowpi_hd44780_send_command(configuration, 0x01);
    delayMicroseconds(5000);    // tom alby says 5ms; adafruit uses 2ms;
                                    // HD44780U datasheet simply says that 0x20 is written to each position;
                                    // SPLC780D datasheet says 1.52ms
}

void cowpi_hd44780_return_home(const cowpi_display_module_protocol_t *configuration) {
    cowpi_hd44780_send_command(configuration, 0x02);
    delayMicroseconds(2000);    // tom alby is silent; adafruit uses 2ms; datasheets agree on 1.52ms
}

void cowpi_hd44780_set_backlight(const cowpi_display_module_protocol_t *configuration, bool backlight_on) {
    is_backlit = backlight_on;
    cowpi_hd44780_send_command(configuration, last_entry_mode);
}

void cowpi_hd44780_set_4bit_mode(const cowpi_display_module_protocol_t *configuration) {
    if (!cowpi_hd44780_send_halfbyte) {
        if (configuration->protocol == SPI) {
            cowpi_hd44780_send_halfbyte = cowpi_hd44780_send_halfbyte_spi;
        } else if (configuration->protocol == I2C) {
            assert(8 <= configuration->i2c_address || configuration->i2c_address < 128);
            cowpi_hd44780_send_halfbyte = cowpi_hd44780_send_halfbyte_i2c;
        } else {}
    }
    /* If initial state is:     8-bit mode      4-bit mode interbyte    4-bit mode intrabyte
     *      then LCD sees...                                                                */
    cowpi_hd44780_send_halfbyte(configuration, 0x3, true);
    /*                                                                  0x3 finishes byte   */
    delayMicroseconds(5000);    // HD44780U datasheet says wait > 4.1ms -- worst case if display was intrabyte
    cowpi_hd44780_send_halfbyte(configuration, 0x3, true);
    /*                          0x33: 8-bit mode    0x33: 8-bit mode                        */
    delayMicroseconds(200);     // HD44780U datasheet says wait more than 100us
    cowpi_hd44780_send_halfbyte(configuration, 0x3, true);
    /*                          0x30: 8-bit mode    0x30: 8-bit mode    0x33: 8-bit mode    */
    delayMicroseconds(200);     // not in the datasheets, but we determined experimentally that 100us is needed
    cowpi_hd44780_send_halfbyte(configuration, 0x2, true);
    /*                          0x20: 4-bit mode    0x20: 4-bit mode    0x20: 4-bit mode    */
    delayMicroseconds(200);     // doesn't seem to be needed ¯\_(ツ)_/¯
}

static void cowpi_hd44780_send_halfbyte_spi(const cowpi_display_module_protocol_t *configuration,
                                            uint8_t halfbyte, bool is_command) {
    uint8_t packet = 0, rs = 0, en = 0;
    bit_order_t bit_order = COWPI_MSB_FIRST;
    if (configuration->adapter_mapping == COWPI_DEFAULT) {
        // unlike I2C, there doesn't seem to be a "typical" mapping for SPI
        // We'll make our lives simpler by using the same default mapping for SPI that we're using for I2C
        /* MSB  P7  P6  P5  P4  P3  P2  P1  P0  LSB *
         * MSB  D7  D6  D5  D4 LITE EN  RW  RS  LSB */
        rs = is_command ? 0 : 1;
        en = 1 << 2;
        packet = rs | (halfbyte << 4) | (is_backlit ? 1 << 3 : 0);
    } else if (configuration->adapter_mapping == ADAFRUIT) {
        // this mapping is used with AdaFruit's SPI+I2C interface
        // https://github.com/adafruit/Adafruit_LiquidCrystal
        /* LSB    QH  QG  QF  QE  QD  QC  QB  QA  MSB *
         * LSB  LITE  D4  D5  D6  D7  EN  RS  xx  MSB */
        rs = is_command ? 0 : 1 << 6;
        en = 1 << 5;
        packet = rs | (halfbyte << 1) | (is_backlit ? 1 : 0);
        bit_order = COWPI_LSB_FIRST;
    } else {}
    cowpi_spi_initialize(configuration, bit_order);
    // place data on the line
    digitalWrite(configuration->select_pin, LOW);
    cowpi_spi_transmit(packet);
    digitalWrite(configuration->select_pin, HIGH);
    // pulse
    digitalWrite(configuration->select_pin, LOW);
    cowpi_spi_transmit(packet | en);
    digitalWrite(configuration->select_pin, HIGH);
    // Tom Alby uses NOPs to get to create an exact 0.5us pulse (6 NOPs (6 cycles) + 1 memory write (2 cycles) = 0.5us)
    // but that isn't portable (also: AVR docs say to use _delay_ms() or _delay_us(), which also aren't portable).
    // However, since we're only doing a half-byte at a time, function calls, etc., will provide sufficient delay.
    // But, just to be sure...
    delayMicroseconds(1);
    // end the pulse
    digitalWrite(configuration->select_pin, LOW);
    cowpi_spi_transmit(packet);
    digitalWrite(configuration->select_pin, HIGH);
    cowpi_spi_finalize();
}

static void cowpi_hd44780_send_halfbyte_i2c(const cowpi_display_module_protocol_t *configuration,
                                            uint8_t halfbyte, bool is_command) {
    uint8_t packet = 0, rs = 0, en = 0;
    if (configuration->adapter_mapping == COWPI_DEFAULT) {
        // this mapping is used with most I2C interfaces and libraries
        // https://github.com/johnrickman/LiquidCrystal_I2C
        // https://github.com/blackhack/LCD_I2C
        /* MSB  P7  P6  P5  P4  P3  P2  P1  P0  LSB *
         * MSB  D7  D6  D5  D4 LITE EN  RW  RS  LSB */
        rs = is_command ? 0 : 1;
        en = 1 << 2;
        packet = rs | (halfbyte << 4) | (is_backlit ? 1 << 3 : 0);
    } else if (configuration->adapter_mapping == ADAFRUIT) {
        // this mapping is used with AdaFruit's SPI+I2C interface
        // https://github.com/adafruit/Adafruit_LiquidCrystal
        /* MSB   GP7 GP6 GP5 GP4 GP3 GP2 GP1 GP0  LSB *
         * MSB  LITE  D7  D6  D5  D4  EN  RS  xx  LSB */
        rs = is_command ? 0 : 1 << 1;
        en = 1 << 2;
        packet = rs | (halfbyte << 3) | (is_backlit ? 1 << 7 : 0);
    } else {}
    cowpi_i2c_initialize(configuration);
    // place data on the line
    cowpi_i2c_transmit(packet);
    // pulse
    cowpi_i2c_transmit(packet | en);
    // Tom Alby uses NOPs to get to create an exact 0.5us pulse (6 NOPs (6 cycles) + 1 memory write (2 cycles) = 0.5us)
    // but that isn't portable (also: AVR docs say to use _delay_ms() or _delay_us(), which also aren't portable).
    // However, since we're only doing a half-byte at a time, function calls, etc., will provide sufficient delay.
    // But, just to be sure...
    delayMicroseconds(1);
    // end the pulse
    cowpi_i2c_transmit(packet);
    cowpi_i2c_finalize();
}