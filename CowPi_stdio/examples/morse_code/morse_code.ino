#include <CowPi_stdio.h>

// Arduino.h is implicitly included in the Arduino IDE but not in PlatformIO
#include <Arduino.h>

FILE *display;

void setup(void) {
    // The C++ approach
    display = cowpi_add_display_module(cowpi_configure_morse_code(7), cowpi_configure_single_pin(LED_BUILTIN));

    // The C approach
    /*
    display = cowpi_add_display_module(
            (cowpi_display_module_t) {
                    .display_module = MORSE_CODE,
                    .words_per_minute = 7
            },
            (cowpi_display_module_protocol_t) {.protocol = NO_PROTOCOL, .data_pin = LED_BUILTIN}
    );
    */
}

void loop(void) {
    fprintf(display, "Hello World\n");
    delay(5000);
}
