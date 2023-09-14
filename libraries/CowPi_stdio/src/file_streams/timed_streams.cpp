/**************************************************************************//**
 *
 * @file timed_streams.cpp
 *
 * @brief Functions supporting the timer interrupt-based symbol buffer, and the
 * externalized symbol buffer itself.
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

/**
 * @brief Interrupt handler for the symbol buffer's timer interrupts.
 *
 * On each invocation, the elapsed time is updated. If enough time has elapsed,
 * the symbol at the head of the buffer is sent to the appropriate display
 * module.
 */
static void timer_handler(void);


#if defined(__AVR_ATmega328P__) || defined (__AVR_ATmega2560__)

#include <avr/interrupt.h>

ISR(TIMER0_COMPB_vect) {
    timer_handler();
}

#elif defined(__MBED__)

#include <chrono>
#include <mbed.h>

static mbed::Ticker displayTicker;

#else

#warning Timer for buffered display modules will not be enabled.

#endif //architecture


uint8_t buffer_head = 0;
uint8_t buffer_tail = 0;
symbol_t symbol_buffer[BUFFER_SIZE];

void cowpi_enable_buffer_timer(void) {
    static bool timer_is_enabled = false;
    if (!timer_is_enabled) {
        timer_is_enabled = true;
#if defined(__AVR_ATmega328P__) || defined (__AVR_ATmega2560__)
        OCR0B = 0x40;   // fires every 1.024ms -- close enough
        TIMSK0 |= (1 << OCIE0B);
#elif defined(__MBED__)
        displayTicker.attach(timer_handler, std::chrono::milliseconds(1));
#else
#warning Timer for buffered display modules will not be enabled.
#endif //architecture
    }
}

static void timer_handler(void) {
    static unsigned int milliseconds = 0;
    static unsigned int next_symbol_change = 1;
    if (!BUFFER_IS_EMPTY) {
        milliseconds++;
        if (next_symbol_change == milliseconds) {
            symbol_t *symbol = symbol_buffer + buffer_head;
            buffer_head = INCREMENT_MODULO(buffer_head, BUFFER_SIZE);
            symbol->callback(symbol);
            milliseconds = 0;
            next_symbol_change = symbol->symbol_duration * SYMBOL_DURATION_SCALING_FACTOR;
        }
    }
}
