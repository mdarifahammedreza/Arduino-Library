/**************************************************************************//**
 *
 * @file communication.h
 *
 * @brief Public-facing type definitions and function pointers for SPI & I2C.
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

#ifndef COWPI_STDIO_COMMUNICATION_H
#define COWPI_STDIO_COMMUNICATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "../typedefs.h"

/**
 * @brief Indicates whether a byte's bits should be transmitted least- or
 * most-significant bit first.
 */
typedef enum {
    COWPI_LSB_FIRST,
    COWPI_MSB_FIRST
} bit_order_t;


/**
 * @brief Function pointer for function that takes care of any setup needed
 * before using SPI to send data to a display module.
 *
 * The particulars of what this setup is depends on the particular SPI
 * implementation.
 *
 * @param configuration the pins used for SPI
 * @param bit_order indicates whether the peripheral expects the byte to arrive
 *      LSB first or MSB first
 *
 * @sa cowpi_spi_transmit
 * @sa cowpi_spi_finalize
 */
extern void (*cowpi_spi_initialize)(const cowpi_display_module_protocol_t *configuration, bit_order_t bit_order);

/**
 * @brief Function pointer for function that uses SPI to send data to a display
 * module.
 *
 * This might be a pre-defined implementation that uses the microcontroller's
 * SPI hardware, or it might be a pre-defined bit-bang implementation.
 *
 * @param byte the byte to be transmitted to the peripheral
 *
 * @sa cowpi_spi_initialize
 * @sa cowpi_spi_finalize
 */
extern void (*cowpi_spi_transmit)(uint8_t byte);

/**
 * @brief Function pointer for function that takes care of any teardown needed
 * after using SPI to send data to a display module.
 *
 * The particulars of what this teardown is depends on the particular SPI
 * implementation.
 *
 * @sa cowpi_spi_initialize
 * @sa cowpi_spi_transmit
 */
extern void (*cowpi_spi_finalize)(void);


/**
 * @brief Assigns to `cowpi_spi_initialize`, `cowpi_spi_transmit`, and 
 * `cowpi_spi_finalize` implementations that use the microcontroller's built-in 
 * SPI hardware.
 *
 * Using the SPI hardware may limit the choice of data and clock pins.
 * Attempting to use the SPI hardware while specifying pins that cannot be used
 * by the SPI hardware may result in unexpected behavior.
 * 
 * The CowPi_stdio library does not have SPI hardware implementations for all 
 * microcontrollers. If a hardware implementation is unavailable, this function 
 * will return `false` and will assign a bit-banged implementation 
 * instead.
 * 
 * @return `true` if hardware implementations have been assigned to 
 *      the CowPi's SPI function pointers
 * @return `false` if hardware implementations have not been assigned
 *      to the CowPi's SPI function pointers
 */
bool cowpi_use_spi_hardware();

/**
 * @brief Assigns to `cowpi_spi_initialize`, `cowpi_spi_transmit`, and 
 * `cowpi_spi_finalize` pure software ("bit-banged") implementations that do not
 * use the microcontroller's built-in SPI hardware.
 *
 * Bit-banged implementations have no restrictions on the choice of data and
 * clock pins.
 */
void cowpi_use_spi_bitbang();


/**
 * @brief Function pointer for function that takes care of any setup needed
 * before using I2C to send data to a display module.
 *
 * This includes, but might not be limited to, transmitting the start bit and
 * the peripheral's address.
 *
 * @param configuration the microcontroller pins, adapter mapping, and possibly
 *      i2c peripheral address
 * @return `true` if ACK received; `false` otherwise
 *
 * @sa cowpi_i2c_transmit
 * @sa cowpi_i2c_finalize
 */
extern bool (*cowpi_i2c_initialize)(const cowpi_display_module_protocol_t *configuration);

/**
 * @brief Function pointer for function that uses SPI to send data to a display
 * module.
 *
 * This might be a pre-defined implementation that uses the microcontroller's
 * SPI hardware, or it might be a pre-defined bit-bang implementation.
 *
 * @param byte the byte to be transmitted to the peripheral
 * @return `true` if ACK received; `false` otherwise
 *
 * @sa cowpi_i2c_initialize
 * @sa cowpi_i2c_finalize
 */
extern bool (*cowpi_i2c_transmit)(uint8_t byte);

/**
 * @brief Function pointer for function that takes care of any teardown needed
 * after using SPI to send data to a display module.
 *
 * This includes, but might not be limited to, transmitting the stop bit.
 *
 * @sa cowpi_i2c_initialize
 * @sa cowpi_i2c_transmit
 */
extern void (*cowpi_i2c_finalize)(void);

/**
 * @brief Reports the address of the I2C peripheral if there is exactly one I2C
 * peripheral attached to the designated pins.
 * 
 * @param i2c_data_pin the I2C serial data pin
 * @param i2c_clock_pin the I2C serial clock pin
 * @return <ul>
 *      <li>0 if no I2C peripherals are detected
 *      <li>-1 if multiple I2C peripherals are detected
 *      <li>the peripheral's I2C address otherwise
 *      </ul>
 */
int8_t cowpi_discover_i2c_address(uint8_t i2c_data_pin, uint8_t i2c_clock_pin);


/**
 * @brief Assigns to `cowpi_i2c_initialize`, `cowpi_i2c_transmit`, and 
 * `cowpi_i2c_finalize` implementations that use the microcontroller's built-in 
 * I2C (aka IIC, aka TWI) hardware.
 *
 * Using the I2C hardware may limit the choice of data and clock pins.
 * Attempting to use the I2C hardware while specifying pins that cannot be used
 * by the I2C hardware may result in unexpected behavior.
 * 
 * The CowPi_stdio library does not have I2C hardware implementations for all 
 * microcontrollers. If a hardware implementation is unavailable, this function 
 * will return `false` and will assign a bit-banged implementation 
 * instead.
 * 
 * @return `true` if hardware implementations have been assigned to 
 *      the CowPi's I2C function pointers
 * @return `false` if hardware implementations have not been assigned
 *      to the CowPi's I2C function pointers
 */
bool cowpi_use_i2c_hardware();

/**
 * @brief Assigns to `cowpi_i2c_initialize`, `cowpi_i2c_transmit`, and 
 * `cowpi_i2c_finalize` pure software ("bit-banged") implementations that do not
 * use the microcontroller's built-in I2C hardware.
 *
 * Bit-banged implementations have no restrictions on the choice of data and
 * clock pins.
 */
void cowpi_use_i2c_bitbang();

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_COMMUNICATION_H
