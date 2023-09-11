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
