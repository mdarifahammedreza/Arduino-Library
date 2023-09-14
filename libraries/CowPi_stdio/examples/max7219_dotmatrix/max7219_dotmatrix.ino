#include <CowPi_stdio.h>

// Arduino.h is implicitly included in the Arduino IDE but not in PlatformIO
#include <Arduino.h>

#ifdef __AVR__
// strictly speaking, avr/pgmspace.h is included by Arduino.h and doesn't need to be explicitly included
#include <avr/pgmspace.h>

#endif //__AVR__

FILE *display;

void setup(void) {
    // The C++ approach
    display = cowpi_add_display_module(
            cowpi_configure_led_matrix_display(25, SOUTH, HEADS),
            cowpi_configure_spi(COWPI_DEFAULT_SPI_SELECT_PIN, COWPI_DEFAULT_SPI_DATA_PIN, COWPI_DEFAULT_SPI_CLOCK_PIN)
    );

    // The C approach
    /*
    display = cowpi_add_display_module(
            (cowpi_display_module_t) {
                    .display_module = LED_MATRIX,
                    .words_per_minute = 25,
                    .display_orientation = SOUTH,
                    .character_flip = HEADS
            },
            (cowpi_display_module_protocol_t) {
                    .protocol = SPI,
                    .data_pin = COWPI_DEFAULT_SPI_DATA_PIN,
                    .clock_pin = COWPI_DEFAULT_SPI_CLOCK_PIN,
                    .select_pin = COWPI_DEFAULT_SPI_SELECT_PIN
            }
    );
    */
}

void loop(void) {
    // On AVR architectures, you can use `fprintf_P` with `PSTR` to put the
    // format string in flash memory, if you want to
#ifdef __AVR__
    fprintf_P(display, PSTR("Hello, World!\t\e\x08\e\x3C\e\xEB\e\xC2\e\xEB\e\x3C\e\x08\t"));
#else
    fprintf(no_scroll_display, "Hello, World!\t\e\x08\e\x3C\e\xEB\e\xC2\e\xEB\e\x3C\e\x08\t");
#endif //__AVR__
    // Those literal bytes kind of looked like a cow face, right?
    // (awkward silence)
    // *Right*?
    fprintf(display, "Hello, World!\n");
}
