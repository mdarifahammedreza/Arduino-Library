/**************************************************************************//**
 *
 * @file spi.h
 *
 * @brief Hardware-based and software-based implementations of the SPI protocol.
 *
 * By default, a software (bit-banged) implementation will be used. When an
 * implementation that uses the microcontroller's hardware is available, it can
 * be used instead by re-assigning the cowpi_spi_initialize(),
 * cowpi_spi_transmit(), and cowpi_spi_finalize() function pointers.
 *
 * The provided implementations are for peripheral-in/controller-out mode.
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

#ifndef COWPI_STDIO_SPI_H
#define COWPI_STDIO_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "communication.h"

/**
 * @brief An implementation of cowpi_spi_initialize() that supports
 * cowpi_spi_transmit_bitbang().
 *
 * @param configuration the pins used for SPI
 * @param bit_order indicates whether the peripheral expects the byte to arrive
 *      LSB first or MSB first
 */
void cowpi_spi_initialize_bitbang(const cowpi_display_module_protocol_t *configuration, bit_order_t bit_order);

/**
 * @brief A bit-banged implementation of cowpi_spi_transmit().
 *
 * @param byte the byte to be transmitted to the peripheral
 */
void cowpi_spi_transmit_bitbang(uint8_t byte);

/**
 * @brief An implementation of cowpi_spi_finalize() that supports
 * cowpi_spi_transmit_bitbang().
 */
void cowpi_spi_finalize_bitbang(void);

/**
 * @brief An implementation of cowpi_spi_initialize() that supports
 * cowpi_spi_transmit_hardware().
 *
 * @param configuration the pins used for SPI
 * @param bit_order indicates whether the peripheral expects the byte to arrive
 *      LSB first or MSB first
 */
void cowpi_spi_initialize_hardware(const cowpi_display_module_protocol_t *configuration, bit_order_t bit_order);

/**
 * @brief An implementation of cowpi_spi_transmit() that uses the
 * microcontroller's hardware.
 *
 * @param byte the byte to be transmitted to the peripheral
 */
void cowpi_spi_transmit_hardware(uint8_t byte);

/**
 * @brief An implementation of cowpi_spi_finalize() that supports
 * cowpi_spi_initialize_hardware().
 */
void cowpi_spi_finalize_hardware(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_SPI_H
