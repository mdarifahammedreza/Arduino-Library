/**************************************************************************//**
 *
 * @file translations.h
 *
 * @brief Functions and definitions to provide a common way to express similar
 *      ubiquitous concepts in the library's code that are expressed slightly
 *      differently on different targets.
 *
 * Obviously when porting this library across different toolchains, concepts
 * such as setting a pin's direction will be expressed differently: `pinMode()`
 * in Arduino and `gpi_set_dir()` in the Raspberry Pi Pico SDK. Even within a
 * single toolchain, there may be differences -- Arduino's `pinMode()` might
 * take a `uint8_t` or a `pin_size_t` for the pin number, and it might take a
 * `uint8_t` or a `PinMode` for the mode (input/output/pullup/pulldown).
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

#ifndef COWPI_STDIO_TRANSLATIONS_H
#define COWPI_STDIO_TRANSLATIONS_H

#include <Arduino.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ARDUINO_ARCH_AVR)
#define pin_number_t uint8_t
#define pin_mode_t uint8_t
#elif defined(ARDUINO_ARCH_MEGAAVR)
#define pin_number_t uint8_t
#define pin_mode_t PinMode
#elif defined(ARDUINO_ARCH_SAMD)
#define pin_number_t pin_size_t
#define pin_mode_t PinMode
#elif defined (ARDUINO_ARCH_MBED)
#define pin_number_t pin_size_t
#define pin_mode_t PinMode
#endif //ARCHITECTURE

// borrowing cowpi_pin_mode from CowPi_stdio -- in an upcoming release, we'll have to figure out how to do this cleaner

/**
 * @brief Configures the specified pin to behave either as an input or an output
 * and, if as an input, then whether it is high-impedance or uses an internal
 * pullup (or pulldown) resistor.
 *
 * @param pin the pin whose mode is to be set
 * @param mode INPUT, OUTPUT, INPUT_PULLUP, or (if supported) INPUT_PULLDOWN
 */
void cowpi_pin_mode(pin_number_t pin, pin_mode_t mode);

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_TRANSLATIONS_H
