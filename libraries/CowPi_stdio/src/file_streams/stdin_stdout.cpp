/**************************************************************************//**
 *
 * @file stdin_stdout.cpp
 *
 * @brief Configures the "Serial Monitor" for `printf()` and `scanf()`.
 *
 * Configures `stdout` and `stdin`, allowing programmers to use `printf()` to
 * write to, and `scanf()` to read from, the USB-based serial interface between
 * the microcontroller and the host computer.
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

#include "file_streams.h"
#include "file_streams_internal.h"

#if defined(ARDUINO)

#include <Arduino.h>

#if defined (__AVR__)
static FILE serial_monitor_allocation;

/**
 * @brief The `get` function that AVR-libc expects for input FILE streams.
 *
 * This function is a wrapper for the more-generally-defined `get` function that
 * glibc and newlib expect for input FILE streams. This function satisfies the
 * AVR-libc `put` interface but simply calls a newlib `get` function that
 * expects an Arduino `Stream` as the cookie, passing the Arduino `Serial` as
 * the cookie.
 *
 * *n.b.*, This function satisfies the `get` argument expected by `fdevopen` and
 * `fdev_setup-stream`, which is nearly identical to the specification for
 * AVR-libc's `getc` and `fgetc` -- the only difference is that this function
 * returns `_FDEV_ERR` for an error and `_FDEV_EOF` for an end-of-file, whereas
 * `getc` and `fgetc` return EOF for both of those conditions.
 *
 * @param filestream
 * @return the character read if it was successfully read; _FDEV_EOF or
 *      _FDEV_ERR otherwise
 */
static int cowpi_stdin_get(FILE *filestream);

/**
 * @brief The `put` function that AVR-libc expects for output FILE streams.
 *
 * This function is a wrapper for the more-generally-defined `put` function that
 * glibc and newlib expect for output FILE streams. This function satisfies the
 * AVR-libc `put` interface but simply calls a newlib `put` function that
 * expects an Arduino `Stream` as the cookie, passing the Arduino `Serial` as
 * the cookie.
 *
 * *n.b.*, This function satisfies the `put` argument expected by `fdevopen` and
 * `fdev_setup-stream` -- specifically, it returns 0 on success. This is in
 * contrast with AVR-libc's `putc` and `fputc` which have the same signature but
 * return the character on success.
 *
 * @param c the character to be displayed
 * @param filestream the FILE stream for the display USB-based serial interface
 * @return 0 if the character is successfully sent to the display module;
 *      non-zero otherwise
 */
static int cowpi_stdout_put(char c, FILE *filestream);

#endif //__AVR__

/**
 * @brief The `get` function that glibc and newlib expect for input FILE
 *      streams.
 *
 * This function satisfies the `readfn`/`read` argument expected by `funopen`
 * and `fopencookie`. The `cookie` should be an Arduino `Stream` object, and
 * this function will read any available characters from that `Stream`, not to
 * exceed `size`.
 *
 * @param cookie the `Serial` object that has the characters to be read
 * @param buffer a destination buffer for those characters
 * @param size the size of the `buffer`
 * @return the number of characters read (0 on EOF)
 */
static int cowpi_arduinostream_get(void *cookie, char *buffer, int size);

/**
 * @brief The `put` function that glibc and newlib expect for output FILE
 *      streams.
 *
 * This function satisfies the `writefn`/`write` argument expected by `funopen`
 * and `fopencookie`. The `cookie` should be an Arduino `Stream` object, and
 * this function will write the characters stored in `buffer` to that `Stream`.
 *
 * @param cookie the `Serial` object that will receive the characters
 * @param buffer an origin buffer for those characters
 * @param size the number of characters to be written
 * @return the number of characters successfully sent to the display module;
 *      -1 on failure
 */
static int cowpi_arduinostream_put(void *cookie, const char *buffer, int size);


void cowpi_stdio_setup(unsigned long bitrate) {
    Serial.begin(bitrate);
    do {
        delay(10);
    } while (!Serial);
    delay(10);
    FILE *serial_monitor;
#if defined (__AVR__)
    // use `fdev_setup_stream` instead of `fdevopen` to avoid `malloc`
    serial_monitor = &serial_monitor_allocation;
    fdev_setup_stream(serial_monitor, cowpi_stdout_put, cowpi_stdin_get, _FDEV_SETUP_RW);
#elif defined (ARDUINO_ARCH_SAMD) || defined (__MBED__)
    serial_monitor = funopen(&Serial, cowpi_arduinostream_get, cowpi_arduinostream_put, NULL, NULL);
    setlinebuf(serial_monitor);
#endif //architecture
    stdin = serial_monitor;
    stdout = serial_monitor;
}

/*
 * NOTES:
 * glibc and newlib expect the `get` function to return the number of characters read.
 * avr-libc expects the `get` function to return the character read.
 * 
 * glibc and newlib expect the `put` function to return the number of characters written.
 * avr-libc expects the `put` function to return 0 for success.
 * 
 * (https://www.gnu.org/software/libc/manual/html_node/Hook-Functions.html)
 * (https://sourceware.org/newlib/docs.html)
 * (https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html)
 */

#if defined (__AVR__)

static int cowpi_stdin_get(__attribute__((unused)) FILE *filestream) {
    char c = '\0';
    int returned_value = cowpi_arduinostream_get(&Serial, &c, 1);
    if (returned_value == -1) {
        return _FDEV_ERR;
    } else if (returned_value == 0) {
        return _FDEV_EOF;
    } else {
        return (int) c;
    }
}

static int cowpi_stdout_put(char c, __attribute__((unused)) FILE *filestream) {
    int returned_value = cowpi_arduinostream_put(&Serial, &c, 1);
    if (returned_value == -1 || returned_value == 0) {
        return -1;
    } else {
        return 0;
    }
}

#endif //__AVR__


static int cowpi_arduinostream_get(void *cookie, char *buffer, int size) {
    int number_of_available_bytes;
    while (!(number_of_available_bytes = ((Stream *) (cookie))->available())) {}
    int i = 0;
    while (i < size && i < number_of_available_bytes) {
        buffer[i] = (char) ((Stream *) (cookie))->read();
        i++;
    }
    return i;
}

static int cowpi_arduinostream_put(void *cookie, const char *buffer, int size) {
    int i = 0;
    // we could just print the whole buffer at once,
    // but we want to insert CR with each LF
    while (i < size) {
        ((Stream *) (cookie))->print(buffer[i]);
        if (buffer[i] == '\n') {
            ((Stream *) (cookie))->print('\r');
        }
        i++;
    }
    return i;
}

#endif //ARDUINO