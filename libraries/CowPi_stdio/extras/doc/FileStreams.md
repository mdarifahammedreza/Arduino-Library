# File Streams

## Creating File Streams

### Standard File Streams

Calling the `cowpi_stdio_setup()` function will initialize the [USB connection to the Serial Monitor](stdin_stdout.md)
or other serial terminal emulator on your host computer and assign `stdin` and `stdout` to that connection.
This function takes one argument, the bitrate (in bps) for the connection -- 9600 is a safe choice;
if your terminal emulator is prepared for a faster connection, you can choose a rate up to 115200.
***Note that some boards will fail silently if the USB cable is not connected, while others will block while waiting for
it to be connected.***

After calling `cowpi_stdio_setup()`, you can use `printf()` and `scanf()`. Note that `cowpi_stdio_setup()` does *not*
specifically assign `stderr`.

### Display Modules

Calling the `cowpi_add_display_module()` will initialize the selected display module and create a file stream to send
text to that display module.
The first argument is a `cowpi_display_module_t` structure that has the details for the display module,
and the second argument is a `cowpi_display_module_protocol_t` that has details for the communication protocol that will
be used to communicate to with the display module.
If successful, the function returns a `FILE *` pointer that can be used with `fprintf()`.
If the function returns `NULL` then the file stream was not created.

The C programming language allows structures to be initialized while skipping over fields whose values are irrelevant or
whose default values are acceptable.
Therefore, in *C* files we recommend calls of the form

```C
FILE *display = cowpi_add_display_module(
            (cowpi_display_module_t) {
                    .display_module = XXX,          // XXX is the type of display module
                    ...other_relevant_fields...
            },
            (cowpi_display_module_protocol_t) {
                    .protocol = YYY,                // YYY is the communication protocol
                    ...other_relevant_fields...
            }
        );
```

The C++ programming language does not allow skipping over fields in `struct` initializers, and so we have provided
convenience functions.
These convenience functions have default argument values where appropriate (which is not an option in C), and so in
*CPP* and *INO* files we recommend calls of the form

```c++
FILE *display = cowpi_add_display_module(
            cowpi_configure_XXX(...arguments...),   // XXX is the type of display module
            cowpi_configure_YYY(...arguments...)    // YYY is the communication protocol
        );
```

### Types of Display Modules

- [MAX7219-driven Seven Segment Display](seven_segment.md)
- [MAX7219-driven LED Dot Matrix Display](led_matrix.md)
- [HD44780-driven LCD Character Display](lcd_character.md)
- [Morse Code](morse_code.md)

### Communication Protocols

#### No Protocol

Principally, the `NO_PROTOCOL` choice is there to be a default value to detect that the application programmer failed to
chose `SPI` or `I2C`.
The `MORSE_CODE` "display," however, legitimately uses neither SPI nor SPI.
If the protocol is `NO_PROTOCOL` and the display module is any option other than `MORSE_CODE`,
then `cowpi_add_display_module()` will return `NULL`.

#### SPI

The Serial-Parallel Interface (SPI) protocol requires that the `data_pin`, `clock_pin`, and the `select_pin` fields be
specified;
however, the `data_pin` and the `clock_pin` have default values, meaning that only the `select_pin` must be specified in
all cases.
If the default bit-banged implementation is used, then the data and clock pins can be any available pins.
If the microcontroller's SPI hardware is used, then your choice of data and clock pins may be limited; however, the
`select_pin` can still be any available pin (this gives you the option of having multiple SPI devices).
Even if the bit-banged implementation is used, the default `data_pin` and `clock_pin` values are the pins that the
microcontroller's hardware uses (or uses by default when the hardware allows options).

For HD44780-based LCD character displays, the `adapter_mapping` field may also be specified;
its default value is `COWPI_DEFAULT`.

##### Terminology

The data pin historically has been called `MOSI` (there is also `MISO`, but not for the purposes of this library).
In 2020, the Open Source Hardware Association (OSHWA) proposed changing this to `SDO` (Serial Data Out) for devices that
are strictly data-out on this pin, and to `COPI` (Controller-Out/Peripheral-In) for devices whose pin direction changes
depending on their role as controller or peripheral.
In 2022, OSHWA changed its proposal to `SDO` and `PICO` (Peripheral-In/Controller-Out) after discovering that the
abbreviation for Controller-In/Peripheral-Out is a vulgar and offensive word in some parts of the world.
Unfortunately, the potential for confusion with the "Pico" shorthand for "Raspberry Pi Pico" dissuades us from
using `PICO`.
Fortunately, this library uses the data pin in only one direction, and so we may refer to it as `SDO` but will typically
refer to it as the "data pin".
The clock pin has been, and continues to be, referred to as `SCK` or `CLK`.
The select pin historically has been called `SS`, and the OSHWA's proposal renames it as `CS` (Chip Select).

As of June 2023, Arduino has adopted "COPI" on the hardware side but still uses "MOSI" on the software side.
The Raspberry Pi Pico uses "TX" (Transmit).
Legacy datasheets still have the legacy terminology.

#### I2C

The Inter-Integrated Circuit (I2C or IIC) protocol, also known as the Two-Wire Interface (TWI) protocol, requires that
the `data_pin` (SDA), `clock_pin` (SCL) and `i2c_address` fields be specified;
however, the `data_pin` and the `clock_pin` have default values, meaning that only the `i2c_address` must be specified
in all cases.
If the default bit-banged implementation is used, then the data and clock pins can be any available pins.
If the microcontroller's SPI hardware is used, then your choice of data and clock pins may be limited.
You have the option of having multiple I2C devices if the devices have different addresses.
Even if the bit-banged implementation is used, the default `data_pin` and `clock_pin` values are the pins that the
microcontroller's hardware uses (or uses by default when the hardware allows options).

For HD44780-based LCD character displays, the `adapter_mapping` field may also be specified;
its default value is `COWPI_DEFAULT`.

##### Specifying the Peripheral's I2C Address

When specifying the display module's's I2C address, you may, of course, hard-code the address if you know it.
Alternatively, if only one peripheral is on the I2C bus, then you may use the `cowpi_discover_i2c_address()` function
inline to assign the address;
see the *scan_i2c* example for a demonstration of `cowpi_discover_i2c_address()`'s functionality.

Note that if there are multiple peripherals on the I2C bus then `cowpi_discover_i2c_address()` does not return a usable
address.
(Similarly, if there are no peripherals on the I2C bus, then it does not return a usable address, either.)
If you need to determine the addresses of multiple peripherals, then we recommend that you run the Arduino Wire library's
[*i2c_scanner*](https://github.com/arduino/ArduinoCore-avr/blob/master/libraries/Wire/examples/i2c_scanner/i2c_scanner.ino)
example to print the addresses of all devices on the I2C bus.

## Notes about `printf`, `fprintf`, and `scanf`

### AVR Program Memory

Owing to AVR microcontrollers' limited RAM, AVR-libc provides a mechanism to place constants in Flash memory and then
use them.
For the purposes of this library, the `PSTR()` macro can be used to place a format string in Flash memory, and the
`printf_P()`, `fprintf_P()`, and `scanf_P()` functions correspond to `printf()`, `fprintf()`, and `scanf()` functions
except that they expect the format string to be in Flash memory.
For example,

```c
printf("The sum of %d and %d is %d\n", i, j, i + j);
```

could be changed to

```c
printf_P(PSTR("The sum of %d and %d is %d\n"), i, j, i + j);
```

reducing the RAM usage by 27 bytes and increasing the Flash usage by 27 bytes.

### ASCII Control Characters

Some ASCII control characters are used to manage output devices.
While modern programmers rarely will see any other than `\t`, `\n`, and perhaps `\r`, there are sensible uses for some
display modules.
For the USB connection to the host computer, most of these are passed through (and may be ignored by the terminal
emulator).
For the display modules controlled by the library, the library determines the effect on the display.
We summarize them here, and they are demonstrated in the example programs.

| Display Module                  |        `\a`         |                                     `\b`                                     |        `\t`         |                `\n`                |                 `\v`                 |           `\f`            |             `\r`             |                             0x1B (gcc `\e`)                             |                 0x7F                 |
|:--------------------------------|:-------------------:|:----------------------------------------------------------------------------:|:-------------------:|:----------------------------------:|:------------------------------------:|:-------------------------:|:----------------------------:|:-----------------------------------------------------------------------:|:------------------------------------:|
| ASCII                           |        bell         |                                  backspace                                   |   horizontal tab    |             line feed              |             vertical tab             |         form feed         |       carriage return        |                                 escape                                  |                delete                |
| nominal CowPi_stdio behavior    |         n/a         | shifts cursor left; next character is inclusive-ORed with existing character | shifts cursor right | clears remaining line, then `\v\r` | places cursor in next row, then `\r` | places cursor in top left | places cursor in left column |                        sends next byte literally                        | `\b`, then clears existing character |
| USB connection to host computer |   passed through    |                                passed through                                |   passed through    |      passed through as `\n\r`      |            passed through            |      passed through       |        passed through        |                             passed through                              |            passed through            |
| 7-segment display (no scroll)   |       ignored       |                                      ✅                                       |          ✅          |                 ✅                  |                  ✅                   |             ✅             |              ✅               | next byte specifies a segment pattern; *see MAX7219 datasheet, Table 6* |                  ✅                   |
| 7-segment display (scrolling)   |       ignored       |                                   ignored                                    | inserts four spaces |        allows line to clear        |                 `\n`                 |           `\n`            |             `\n`             | next byte specifies a segment pattern; *see MAX7219 datasheet, Table 6* |               ignored                |
| LED matrix display (scrolling)  |       ignored       |                                   ignored                                    | inserts ten columns |      inserts 2×width columns       |                 `\n`                 |           `\n`            |             `\n`             |                  next byte specifies a column pattern                   |               ignored                |
| LCD character display           |   prints CGRAM[7]   |                               prints CGRAM[8]                                |          ✅          |                 ✅                  |                  ✅                   |             ✅             |              ✅               |                            prints CGROM[27]                             |          prints CGROM[127]           |
| Morse Code                      | end of message (KA) |                                  error (HH)                                  |   interword space   |         new paragraph (BT)         |            next line (AA)            |    end of message (AR)    |           ignored            |                                 ignored                                 |              error (HH)              | 

[//]: # (Consider having MAX7219 go into test mode / shutdown mode / test mode / shutdown mode / normal operation in response to `\a`)

## Special Functions

This library provides three functions that take a `FILE *` argument and performs certain actions on the display.
The `cowpi_clear_dispaly(FILE *)`, `cowpi_sleep_display(FILE *)`, and `cowpi_wake_display(FILE *)` functions perform
actions that otherwise would require using lower-level `cowpi_max7219_XXX` or `cowpi_hd44780_XXX` functions but do so in
a way that do not risk unpredictable behavior.
We summarize them here.

|        Display Module        |                  `cowpi_clear_display`                  |                         `cowpi_sleep_display`                         |         `cowpi_wake_display`          |
|:----------------------------:|:-------------------------------------------------------:|:---------------------------------------------------------------------:|:-------------------------------------:|
| nominal CowPi_stdio behavior | Removes all displayed characters and performs form feed | Places display in low-power state while remembering displayed content | Restores display from low-power state |
|      7-segment display       |                            ✅                            |                   Places MAX7219 in "shutdown" mode                   | Takes MAX7219 out of "shutdown" mode  |
|      LED matrix display      |         completes print and performs form feed          |                   Places MAX7219 in "shutdown" mode                   | Takes MAX7219 out of "shutdown" mode  |
|    LCD character display     |                            ✅                            |                          Disables backlight                           |           Enables backlight           |
|          Morse Code          |                         ignored                         |                                ignored                                |                ignored                |

