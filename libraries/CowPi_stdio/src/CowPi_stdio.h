/*!
 * @mainpage CowPi_stdio Library
 *
 * @version 0.6.0
 *
 * @copyright (c) 2022-23 Christopher A. Bohn
 *
 * @section overview Overview
 *
 * This library is designed to support the use of idiomatic C input/output
 * functions with microcontroller boards.
 *
 * @section hardware Supported Hardware
 *
 * The current version supports:
 *  - Microcontrollers
 *      - ATmega328P    (Arduino Uno, Arduino Nano)
 *      - ATmega2560    (Arduino Mega 2560)
 *      - ATmega4809    (Arduino Uno Wifi Rev 2, Arduino Nano Every) -- limited support
 *      - RP2040        (Raspberry Pi Pico (Arduino Framework),
 *                       Arduino Nano RP2040 Connect)
 *      - SAM D21       (Arduino Zero, Arduino Nano 33 IoT) -- limited support
 *  - Inputs/Outputs
 *      - USB connection to host computer (`stdin`/`stdout`)
 *      - Morse Code to output pin (LED or buzzer with oscillator crystal)
 *      - MAX7219-based 8-digit, 7-segment display
 *      - MAX7219-based 8x8 LED matrix display
 *      - HD44780-based LCD character display
 *
 * @section license License
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

/**************************************************************************//**
 *
 * @file CowPi_stdio.h
 *
 * @author Christopher A. Bohn
 *
 * @brief Functions to configure file streams for `stdio.h` functions.
 *
 * Configures the `stdin` and `stdout` streams to work with the Arduino Serial
 * Monitor. Configures display modules and provides file streams to display text
 * on those display modules using `fprintf()`.
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

#ifndef COWPI_STDIO_H
#define COWPI_STDIO_H

/* Public-facing function prototypes */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "typedefs.h"
#include "communication/communication.h"
#include "file_streams/file_streams.h"
#include "fonts/fonts.h"
#include "hd44780/hd44780.h"
#include "max7219/max7219.h"
#include "display_functions.h"

#define COWPI_STDIO_VERSION ("0.6.0")

#ifdef __cplusplus
} // extern "C"
#endif

#endif //COWPI_STDIO_H
