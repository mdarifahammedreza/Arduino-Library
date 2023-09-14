# USB Connection to Host Computer

Provides a convenient way to print to, and read inputs from, a terminal emulator that is more idiomatic and familiar to
C programmers than `Serial.println()` and `Serial.read()`.

![Printf and Scanf Demonstration](img/printf_and_scanf.gif)

## Examples

### printf_and_scanf

The *printf_and_scanf* example demonstrates setting up the connection to the host computer, printing to the terminal
emulator, and reading from the terminal emulator.
Placing format strings in Flash memory is also demonstrated.

```c++
void setup(void) {
    cowpi_stdio_setup(9600);

    printf("With the CowPi_stdio library, you can use `printf` and `scanf`,\n");
    printf("just like you're used to.\n\n");
#ifdef __AVR__
    printf_P(PSTR("On AVR architectures, you can move the format strings to Flash memory by using\n"));
    printf_P(PSTR("the `PSTR` macro to put the string in Program Flash memory, and the `printf_P`\n"));
    printf_P(PSTR("function as the `printf` equivalent that uses strings from Program Flash memory.\n\n"));
#if defined ARDUINO_ARCH_AVR
    printf_P(PSTR("Programs on your microcontroller might benefit from using `PSTR` & `printf_P`.\n"));
    printf("Otherwise, the format strings will consume Data SRAM memory.\n\n");
#elif defined ARDUINO_ARCH_MEGAAVR
    printf("On newer megaAVR architectures, using `PSTR` and `printf_P` may be used, but\n");
    printf("since program memory is mapped into the data address space, the compiler will\n");
    printf("place string constants in program memory, and the regular `printf` function can\n");
    printf("access them.\n\n");
#endif //ARDUINO_ARCH_AVR or ARDUINO_ARCH_MEGAAVR
#endif //__AVR__
}

void loop(void) {
    char c;
    int i;
    printf("Enter an integer from the host computer's keyboard: ");
    scanf("%d", &i);
    scanf("%c", &c);    // consume the linefeed character (or other non-digit character)
    printf("\nYou entered: %d\n", i);
}
```

### printf_limitations

To keep the size of the program as small as possible, most microcontrollers' implementations of `vfprintf` limit the
less-commonly used conversions.
The *printf_limitations* demonstrate these:

- No floating point conversions (prints without converting the specifier) on most targets
    - This can be overcome with compiler arguments, but it will significantly increase the size of your program (by
      about 1.4KB on AVR targets)
    - Floating point conversions supported on Raspberry Pi Pico
- No 64-bit integer conversions (aborts print on AVR targets; prints without converting the specifier on ARM targets)
- AVR does not support specifiers with widths and precisions greater than 255 (truncates the width and precision to 8
  bits)
- AVR does not support specifiers with variable width and precision (aborts the print)
    - The *hd44780_blinky* example demonstrates the occasional need to work around this limitation

```c++
#include <CowPi_stdio.h>

// Arduino.h is implicitly included in the Arduino IDE but not in PlatformIO
#include <Arduino.h>

void setup(void) {
    cowpi_stdio_setup(9600);

    float f = 3.14f;
    long long n = 42;

    printf("The default `printf` ");
#ifdef ARDUINO_RASPBERRY_PI_PICO
    printf("will");
#else
    printf("won't");
#endif //ARDUINO_RASPBERRY_PI_PICO
    printf(" convert floating point values such as %f on your microcontroller.\n", f);

#ifdef __AVR__
    printf("If you need to print floats, use the additional build arguments\n");
    printf("        -Wl,-u,vfprintf -lprintf_flt -lm        (see the `extras/` directory)\n\n");
#else
    printf("\n");
#endif //__AVR__

    printf("Trying to print 64-bit integers such as %lld (which is more bits than we need)\n", n);
#ifdef __AVR__
    printf("will abort the output for that `printf` statement.\n\n");
#else
    printf("won't result in a conversion.\n\n");
#endif //__AVR__

    printf("Here's what happens with a specifier's width (or precision) that is greater than 255:\n");
    printf("Using \"%c5d\"\t>>>%5d<<<\tworks as expected.\n", '\%', 42);
#ifdef __AVR__
    printf("Using \"%c260d\"\t>>>%260d<<<\ttruncates the width to 8 bits, yielding a width of 4.\n\n", '\%', 42);
#else
    printf("Using \"%c260d\"\t>>>%260d<<<\talso works as expected.\n\n", '\%', 42);
#endif //__AVR__

    printf("Trying to use a variable-width conversion, such as \"%*s\"\n", 3, "\%*s");
#ifdef __AVR__
    printf("will abort the output for that `printf` statement.\n");
#else
    printf("won't be a problem on your microcontroller.\n");
#endif //__AVR__
}

void loop(void) {
}
```

A quirk of avr-gcc is that it expects `%f` to convert a double rather than either a double or a float, generating a
compiler warning.
What makes this quirk particularly interesting is that on the 8-bit AVR microcontrollers, both `float` and `double`
correspond to the IEEE754 "binary32" (aka "single precision") type.
