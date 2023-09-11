#include <CowPi_stdio.h>

// Arduino.h is implicitly included in the Arduino IDE but not in PlatformIO
#include <Arduino.h>

#ifdef __AVR__
// strictly speaking, avr/pgmspace.h is included by Arduino.h and doesn't need to be explicitly included
#include <avr/pgmspace.h>

#endif //__AVR__

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
