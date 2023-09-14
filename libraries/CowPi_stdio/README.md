# CowPi_stdio Library

[![GitHub Release](https://img.shields.io/github/release/DocBohn/CowPi_stdio)](https://github.com/DocBohn/CowPi_stdio/releases)
[![GitHub License](https://img.shields.io/github/license/DocBohn/CowPi_stdio)](https://github.com/DocBohn/CowPi_stdio/blob/main/LICENSE)

[![Arduino Library](https://www.ardu-badge.com/badge/CowPi_stdio.svg)](https://www.ardu-badge.com/CowPi_stdio)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/docbohn/library/CowPi_stdio.svg)](https://registry.platformio.org/libraries/docbohn/CowPi_stdio)

[//]: # ([![See also CowPi]&#40;https://img.shields.io/badge/🐮_𝜋_see_also-CowPi-crimson&#41;]&#40;https://github.com/DocBohn/CowPi&#41;)
[//]: # (scarlet)
[![See also CowPi](https://img.shields.io/badge/🐮_𝜋_see_also-CowPi-rgb(255,36,0))](https://github.com/DocBohn/CowPi)

## What the CowPi_stdio library has to offer

This library brings `FILE` streams to the Arduino toolchain.
Specifically, the library defines `stdout` and `stdin` file streams to use the USB connection to a host computer.
Doing so brings `printf` and `scanf`, familiar to any C programmer, to Arduino boards.
The library also provides `FILE` streams for assorted display modules, allowing programmers to use `fprintf` to send
text to the display modules

## What the CowPi_stdio library will have to offer

Our plans for upcoming releases include:

- File streams for display modules in the Raspberry Pi Pico SDK framework
- File streams for UART devices that are not (necessarily) connected to USB
- File streams for additional display modules

## How to use the CowPi_stdio library

See [the documentation](https://cow-pi.readthedocs.io/en/latest/stdio.html) for details.
The short version, assuming you're using C++ (to include Arduino's `.ino` files) is:

- To set up `stdout` and `stdin`, call the `cowpi_stdio_setup()` function with the desired bit rate.
- For the display modules, call `cowpi_add_display_module()` and assign the pointer it returns to a `FILE *` variable.
  The first argument has the configuration details for the display module, and the second argument has the configuration
  details for the communication protocol used to send data to the display module.

  ```c++
  FILE *display = cowpi_add_display_module(
      cowpi_configure_XXX(...arguments...),   // XXX is the type of display module
      cowpi_configure_YYY(...arguments...)    // YYY is the communication protocol
    );
  ```

## Enabling/Disabling Memory-Expensive Display Modules

While the library is written for run-time configuration, there are some portions that you may wish to eliminate at compile-time to reduce the memory used.
You can do so by passing compile-time arguments that are discussed below:

### Matrix Font

The dot matrix font defined in the library is sizable indeed.
Removing the dot matrix font will eliminate a little over 2KB -- on AVR devices, this savings will be in flash memory; on ARM devices, this savings will be in RAM

- `-DNO_MATRIX_FONT` explicitly excludes the dot matrix font and any display modules that depend upon it;
  this is the default for the ATmega328P (Arudino Uno, Arduino Nano)
- `-DMATRIX_FONT` explicitly includes the dot matrix font;
  this is the default for all other microcontrollers

### Morse Code Font

The Morse Code font defined in the library is smaller than the dot matrix font but large enough to consider excluding.
Removing the Morse Code font will eliminate a little over 1KB -- on AVR devices, this savings will be in flash memory; on ARM devices, this savings will be in RAM

- `-DNO_MORSE_FONT` explicitly excludes the Morse Code font and the Morse Code display;
  this is the default for the ATmega328P (Arudino Uno, Arduino Nano)
- `-DMORSE_FONT` explicitly includes the Morse Code font;
  this is the default for all other microcontrollers

### Timed Displays

There are some displays that update based on a timer, such as the scrolling 7-segment display, the LED matrix display, and the Morse Code display.
Passing the compiler argument `-DNO_TIMED_DISPLAYS` will disable these displays and will elimate 880 bytes from flash memory.
This argument is *not* the default on any microcontrollers.

### Which displays are disabled

- The scrolling 7-segment display is disabled when `NO_TIMED_DISPLAYS` is defined
- The LED matrix display is disabled when either `NO_TIMED_DISPLAYS` or `NO_MATRIX_FONT` is defined
- The Morse Code display is disabled when either `NO_TIMED_DISPLAYS` or `NO_MORSE_FONT` is defined
- All other displays are always enabled

If you attempt to configure a disabled display module, then the `FILE *` variable that `add_display_module()` returns will be `NULL`, the same as would happen for any other configuration error.

## Status

| MCU                        | `printf`/`scanf` | SPI bitbang | SPI hardware | I2C bitbang | I2C hardware |   Buffer Timer    | Notes                                                                                                                             |
|:---------------------------|:----------------:|:-----------:|:------------:|:-----------:|:------------:|:-----------------:|:----------------------------------------------------------------------------------------------------------------------------------|
| ATmega328P                 |        ✅         |      ✅      |      ✅       |      ✅      |      ✅       |  uses registers   | I2C code works fine on actual hardware but not in simulator (bitbang I2C works for both)                                          |
| ATmega2560                 |        ✅         |      ✅      |      ✅       |      ✅      |      ✅       |  uses registers   | I2C code works fine on actual hardware but not in simulator (bitbang I2C works for both)                                          |
| ATmega4809                 |        ✅         |      ⁇      |      ❌       |      ⁇      |      ❌       |         ❌         |                                                                                                                                   |
| nRF52840                   |        ❌         |      ⁇      |      ❌       |      ⁇      |      ❌       |         ⁇         | Locks up USB -- problem with waiting for Serial? <!-- https://forum.arduino.cc/t/nano-33-ble-sense-serial-workaround/884962/7 --> |
| RP2040 (Arduino framework) |        ✅         |      ✅      |      ❌       |      ✅      |      ❌       | uses mbed::Ticker | Still need to resolve floating point conversions                                                                                  |
| SAM D21                    |        ✅         |      ⁇      |      ❌       |      ⁇      |      ❌       |         ❌         | Still need to resolve floating point conversions                                                                                  |

[//]: # (| RA4M1                      |        ⁇         |      ⁇      |      ❌       |      ⁇      |      ❌       |         ⁇         |                                                                                                                                   |)

| Display Module                                                      | AVR | megaAVR | MBED | SAMD |
|:--------------------------------------------------------------------|:---:|:-------:|:----:|:----:|
| 8-digit, 7-segment display (MAX7219, 5V)                            |  ✅  |    ⁇    |  ✅   |  ⁇   |
| 8-digit, 7-segment scrolling display (MAX7219, 5V)                  |  ✅  |    ❌    |  ✅   |  ❌   |
| 8x8 LED matrix scrolling display (MAX7219, 5V)                      |  ✅  |    ❌    |  ✅   |  ❌   |
| 16x2 LCD character display (HD44780, 5V; some devices support 3.3V) |  ✅  |    ⁇    |  ✅   |  ⁇   |
| 20x4 LCD character display (HD44780, 5V; some devices support 3.3V) |  ✅  |    ⁇    |  ✅   |  ⁇   |
| 128x64 OLED matrix display (SSD1306, 3.3V or 5V)                    |  ❌  |    ❌    |  ❌   |  ❌   |
| 128x32 OLED matrix display (SSD1306, 3.3V or 5V)                    |  ❌  |    ❌    |  ❌   |  ❌   |
| Morse Code LED (no serial adapter necessary)                        |  ✅  |    ❌    |  ✅   |  ❌   |

[//]: # (| 8x12 LED matrix scrolling display &#40;Uno R4 Wifi&#41;                   |  ❌  |    ❌    |  ❌   |  ❌   |)

(MBED tested on Raspberry Pi Pico but not on Arduino Nano 33 BLE)

### Tested on...

- AVR
  - AVR
    - Atmel ATmega328P
      - [x] Arduino Uno R3
      - [x] Arduino Nano
    - Atmel ATmega2560:
      - [x] Arduino Mega 2560
  - megaAVR
    - Atmel ATmega4809
      - [ ] Arduino Uno Wifi Rev 2
      - [x] Arduino Nano Every
- ARM
  - Mbed OS
    - Nordic nRF52840
      - [x] Arduino Nano 33 BLE (see notes, above)
    - Raspberry Pi RP2040
      - [ ] Arduino Nano RP2040 Connect
      - [x] Raspberry Pi Pico (Arduino platform)
  - SAMD
    - Atmel SAM D21:
      - [x] Arduino Nano 33 IoT
      - [ ] Arduino Zero

[//]: # (  - RENESAS)

[//]: # (    - [ ] Renesas RA4M1: Arduino Uno R4)

## The tradeoffs

### Advantage of using `printf`

Being able to print using a format string with conversion specifiers is *much* more convenient than using the
Arduino `Serial.print` and `Serial.println` functions.
Even without specifying alignment and padding, as soon as you combine constant text with a variable, `printf` becomes
easier to use than a chain of `Serial.print`/`println` calls.
(But, of course, you have all the conversion specifier goodness, including alignment and padding.
Well, *almost* all -- see the note below about [limitations](#Limitations).)

### Disadvantage of using `printf`

Using `printf` *will* increase the size of your executable by about 1.2KB over simply using `Serial.print`
and `Serial.println`, about the same as if you were to combine `snprintf` with `Serial.println`.

For example, on an Arduino Nano,

```cpp
void setup(void) {
    Serial.begin(9600);
    int a = 3, b = 7;
    Serial.print(a);
    Serial.print(" + ");
    Serial.print(b);
    Serial.print(" = ");
    Serial.println(a + b);
}

void loop(void) {
}
```

uses 1814 bytes of Flash memory and 196 bytes of SRAM, whereas,

```cpp
#include <CowPi_stdio.h>

void setup(void) {
    cowpi_stdio_setup(9600);
    int a = 3, b = 7;
    printf("%d + %d = %d\n", a, b, a + b);
}

void loop(void) {
}
```

uses 3054 bytes of Flash memory and 218 bytes of SRAM.

### Neither advantage nor disadvantage

- Even without using the CowPi_stdio library, you can create formatted output by using `sprintf` (or, better
  yet, `snprintf`).
  This, however, still brings the code for format conversions into the program:

  ```cpp
  void setup(void) {
      Serial.begin(9600);
      int a = 3, b = 7;
      char string[11] = { 0 };
      snprintf(string, 11, "%d + %d = %d", a, b, a + b);
      Serial.println(string);
  }

  void loop(void) {
  }
  ```

  uses 3066 bytes of Flash memory and 200 bytes of SRAM.
- Conventional wisdom is that format conversions are very slow.
  While it is true that using `printf` is slower than not printing to a terminal, it is *not* true that using `printf`
  is slower than chaining `Serial.print`/`println` calls.
  The `Serial.print` and `Serial.println` statements in the first snippet require 276µs (±4µs) to execute, and
  the `printf` statement in the second snippet requires 212µs (±4µs) to execute.
- Unsurprisingly, the display modules will demand a little more memory.
  Some display modules, such as scrolling text on the LED dot matrix display will demand a significant fraction of the
  available memory.
  You are unlikely to save much memory by implementing the equivalent functionality without the stdio framework.

## Limitations

The only limitations are inherited from the avr-libc (for AVR) and the newlib (for ARM) libraries.
The one you will most likely notice is floating point conversions, which can be overcome.
There are other limitations that cannot be.

### Floating point conversions

Like most microcontroller environments, the default implementation does not support floating point conversions (except
for Raspberry Pi Pico).
Instead, the output will be `?` on AVR architectures.
On ARM (SAMD) architectures, the output is an unprintable character.

Another implementation is available that will support floating point conversions.

- For AVR architectures, the richer implementation is available through a compiler (linker) option:

  ```
  -Wl,-u,vfprintf -lprintf_flt -lm
  ```

  If you're using the Arduino IDE, you'll need to set up a `platform.local.txt` file;
  if you're using PlatformIO, you can add the build flags to your `platform.ini` file.
  ***Note**: the richer implementation will add about 1.4KB to your executable.*

- We're still determining how to get the richer implementation for ARM (SAMD).

<!--
    Try `-Wl,-u,_printf_float` (and also `-Wl,-u,_scanf_float`)
    -or-
    in the source code
    `asm (".global _floatf_float");`
    `asm (".global _scanf_float");`
-->

### Other limitations

- The specified width and precision can be at most 255 on AVR architectures.
  This is unlikely to be a practical limitation.
- `long long` conversions (*i.e.*, 64-bit integers) are not supported.
  The `%lld` conversion specifier will abort the output on AVR architectures.
  On ARM architectures, `ld` is output.
  This also is unlikely to be a practical limitation.
- Variable width or precision fields is not supported on AVR architectures.
  Using `*` to specify the width or precision will abort the output.
  Lines 105-108 and 119-122 of *hd44780_blinky* have code that must work around this limitation.

## About the name

Some of the code in the CowPi_stdio library was once part of the [CowPi](https://github.com/DocBohn/CowPi/) library,
which was designed to work with Cow Pi development boards.
The code in the CowPi_stdio library will work in projects that do not use a Cow Pi development board, but we preserve
the "CowPi" part of the name as a nod to its origins, and also to distinguish it from the `stdio` portion of libc.
The "stdio" part of the name is because it makes available to AVR architectures two of the most-commonly used functions
from `stdio.h` and makes it possible for coders to use stdio functions to work with display modules for both AVR and ARM
architectures.

### Why *Cow Pi*?

Because it's punny.

### No, I mean, *-Pi* is typically used as a suffix for circuits that use a Raspberry Pi instead of an Arduino.

Typically, yes, but *Cowduino* isn't very punny, is it?
Besides, it also works with the Raspberry Pi Pico.

## An abbreviated pre-history

Some of the code in the CowPi_stdio library was once part of the [CowPi](https://github.com/DocBohn/CowPi/) library,
which was designed to work with Cow Pi development boards, designed for class assignments at the hardware/software
interface.
Version 0.3 of the CowPi library saw many improvements, including being able to use `printf()` and `scanf()` with a
serial terminal and abstractions for controlling MAX7219- and HH44780-based display modules.
Plans for v0.5 included abstractions for SSD1306-based display modules and to further abstract the display modules by
creating file streams for them that can be used with `fprintf()`.
(The `fprintf()` code is in CowPi_stdio; the SSD1306 code will probably be in CowPi_stdio v0.7.)
As we were making initial forays into what this display code would look like, we realized that the code that controls
the displays depends on the displays but not on any of the other hardware on the Cow Pi development board, and we
realized that it might be useful for projects that don't use the Cow Pi development board.

And so we separated the display code out from the rest of the CowPi library.
