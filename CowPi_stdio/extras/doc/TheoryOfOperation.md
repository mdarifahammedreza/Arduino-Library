# Theory of Operation

## Communication Protocols

The theory of operation for [I2C](https://www.nxp.com/docs/en/user-guide/UM10204.pdf) and
[SPI](https://www.analog.com/media/en/analog-dialogue/volume-52/number-3/introduction-to-spi-interface.pdf)
are documented in their datasheets and are not further elaborated upon here.
The hardware implementations are based on the microcontrollers' datasheets:

- [Atmel ATmega328P](http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf)
- [Atmel ATmega2560](https://ww1.microchip.com/downloads/en/devicedoc/atmel-2549-8-bit-avr-microcontroller-atmega640-1280-1281-2560-2561_datasheet.pdf)
- [Atmel ATmega4809](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega4808-4809-Data-Sheet-DS40002173A.pdf)
- [Atmel SAM D21](https://ww1.microchip.com/downloads/en/DeviceDoc/SAM_D21_DA1_Family_DataSheet_DS40001882F.pdf)
- [Nordic nRF52840](https://infocenter.nordicsemi.com/pdf/nRF52840_PS_v1.1.pdf)
- [Raspberry Pi RP2040](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

## Display Modules

The lower-level control of the display modules is based on their datasheets:

- [Maxim MAX7219](https://www.analog.com/media/en/technical-documentation/data-sheets/max7219-max7221.pdf)
- [Hitachi HD44780](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf)
- [Solomon Systech SSD1306](https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)

## File Streams

In the current version, the `stdin`/`stdout` FILE streams are implemented distinctly from those for the discrete display
modules.
This is consequent from the FILE streams for the display modules to be a recent introduction to the CowPi / CowPi_stdio
libraries.
We anticipate harmonizing the `stdin`/`stdout` code with the display modules' code in a future release.

### Creating File Streams on AVR and ARM

While [AVR-libc](https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html) has `fdev_open(put, get)` that
returns a `FILE *`, we choose to use `fdev_setup_stream(FILE *, put, get, rwflag)` instead.
Given the limited program memory available, we opt for statically-allocated `FILE` variables instead of
bringing `malloc()` into the program's footprint.
For `stdin`/`stdout`, the statically-allocated `FILE` variable for AVR targets is simply a scalar `FILE`.
For display modules, it is part of a richer data structure, described below.

For ARM targets we use [newlib](https://sourceware.org/newlib/libc.html#funopen)'s
`funopen(cookie, readfn, writefn, seekfn, closefn)` that returns a `FILE *`.
Both [newlib](https://sourceware.org/newlib/libc.html#fopencookie)
and [glibc](https://www.gnu.org/software/libc/manual/html_node/Streams-and-Cookies.html)
define a similar `fopencookie()` function, but it doesn't appear to be always available.

The AVR-libc `put` and `get` function pointers expect functions that operate on one character at a time.
The newlib/glibc `readfn` and `writefn` function pointers expect functions that can operate on an arbitrary number of
characters.
Another notable difference is the `cookie` used by the newlib/glibc code.
A cookie "is an object ... which records where to fetch or store the data read or written. It is up to (the designer
setting up a FILE stream) to define a data type to use for the cookie. The stream functions in the library never refer
directly to its contents, and they don’t even know what the type is; they record its address with type `void *`."

We define `put` and `get` functions that act as wrappers for `writefn` and `readfn` functions.
Because the `writefn` and `readfn` functions operate on an arbitrary number of characters, they can also operate on one
character at a time.
For `stdin`/`stdout`, the cookie is the Arduino `Serial` object.
For display modules, it is a custom data structure, described below.

### Display Modules' Cookie

We define a data structure, `stream_data_t` that has four required fields and one optional field.
We then create a static array of a limited number of these structures -- currently 5, which is more display modules than
seems likely for most circuits.
The fields are:

- `stream`
    - For AVR targets, `stream` is of type `FILE`, and its address is passed to `fdev_setup_stream()`
    - For ARM targets, `stream` is of type `FILE *`, and it points to the FILE stream returned by `funopen()`
    - For AVR targets, `stream`'s address is used to retrieve the cookie for every use of the display module
    - For ARM targets, the address stored within `stream` is used to retrieve the cookie only for the special
      functions `cowpi_clear_display()`, `cowpi_sleep_display()`, and `cowpi_wake_display()`
- `put` -- this is a file pointer to the `writefn` function provided to `funopen()` and wrapped
  by `fdev_setup_stream()`'s `put` function
- `display_module` -- this is a copy of the `cowpi_display_module_t` argument to `cowpi_add_display_module()` that
  contains relevant information about the display module
- `configuration` -- this is a copy of the `cowpi_display_module_protocol_t` argument to `cowpi_add_display_module()`
  that contains relevant information about the communication protocol
- `ms_per_signal` -- this field is used only for display modules with scrolling text and for Morse Code; it indicates
  how much time should pass between updates to the display

All display modules use the same AVR `put` function, `cowpi_display_module_put()`.
The FILE stream is converted to a cookie.
For AVR targets, because the statically-allocated `FILE stream` is the first field in the `stream_data_t`
structure, `stream`'s address is also the cookie's address;
this conversion is simply casting `cowpi_display_module_put()`'s `FILE *` argument to `stream_data_t *`.
`cowpi_display_module_put()` then calls `stream_data->put()`, which is the `writefn` function appropriate for the
particular display module.

For ARM targets, the appropriate `writefn` function is invoked directly.

### Buffered Display Modules

In the interest of minimizing the program's footprint, non-scrolling displays do not use a buffer.
We accept the possible brief loss of responsiveness in exchange for a smaller program -- if you're using this library
for an application with hard real-time constraints, you may wish to reconsider.

Some display must use a buffer -- specifically, scrolling text on a 7-segment buffer display, scrolling text on an LED
matrix, and Morse Code.
All such displays (if you have more than one in your circuit) share a single buffer.
Microcontrollers with more memory will have a larger buffer, and microcontrollers with little memory will have a small
buffer.

For buffered displays, the `writefn` function creates a `symbol_t` variable and adds it to the buffer.
If, and only if, the buffer is full, then the function blocks until there is room for the next symbol.
The `symbol_t` structure has four fields:

- `callback` -- a function pointer to the function that will actually send the symbol to the display module;
  while its parameter is declared to be `void *`, the argument is assumed to be `symbol_t *`
- `stream_data` -- a pointer to the cookie
- `symbol` -- the byte to be sent to the display module
- `symbol_duration` -- 1/8 of the number of milliseconds before the *next* symbol should be sent to the display
  module --
  by scaling the duration, some timing precision is lost;
  however, we gain the ability to have up to 2 seconds between symbols without wasting most of a second byte

A timer handler removes the next symbol from the buffer and calls `symbol->callback()`.
The callback function extracts the display module and communication protocol data from the cookie contained within
the `symbol_t` argument and uses this to send the symbol itself to the display module.

For ATmega328P and ATmega2560 microcontrollers, we use Output Comparison B on Timer0, the same timer that the Arduino
framework has already set to overflow every 1.024ms.
In doing so, we leave Timer1 and Timer2 available for application programs, and we do not make any changes that could
affect Arduino's `millis()` or `delay()` functions.

For the RP2040 microcontroller (and probably the nRF52840 microcontroller), we use the Mbed OS `Ticker.attach()`
function to configure the timer interrupts.

Buffered displays are not yet implemented for other targets.

### Special Functions

We expect that clearing the display, and placing it into and out of a low-power state, are the only functions that might
drive an application programmer to try to use the display module's lower-level functions ("only", except for creating
custom characters for HD44780 devices -- perhaps we will remedy this in a future release).
As these have the possibility of producing undesirable behavior from the FILE stream, *particularly* clearing the
display, we provide functions to perform these actions in a manner that will keep the FILE stream in a predictable
state.

These functions take a `FILE *` argument and determine which display module should be acted upon.
This is accomplished by obtaining the appropriate cookie and comparing the `put` function pointer to the addresses of
the `writefn` functions for each of the display modules.
On AVR microcontrollers, obtaining the cookie is again a simple matter of casting the `FILE *` argument
to `stream_data_t *`.
For ARM microcontrollers, we iterate over the array of cookies (being a very small array, this happens quickly) and
compare the `FILE *` argument to the `FILE *stream` field in each cookie to determine which is the correct cookie.
