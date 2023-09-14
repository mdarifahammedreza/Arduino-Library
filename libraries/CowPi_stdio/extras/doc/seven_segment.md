# MAX7219-driven Seven-Segment Display

The [MAX7219](https://www.analog.com/media/en/technical-documentation/data-sheets/max7219-max7221.pdf) is "serial
input/output common-cathode display drivers that interface microprocessors (µPs) to 7-segment numeric LED displays of up
to 8 digits, bar-graph displays, or 64 individual LEDs."
You can readily find MAX7219-driven
[8-digit 7-segment display modules](https://www.google.com/search?q=max7219+8-digit+7-segment+display) for a couple of
dollars online.
The mapping of bits to LEDs for this application is well-defined in the datasheet, and once the display module is
properly connected to your microcontroller board, this library should work with it without any customization.
The only "gotcha" (and this really is beyond the scope of the library) is that the MAX7219 is strictly a 5V device (with
reasonable allowances), so if your microcontroller board outputs 3.3V for logic high then you will need a "level
converter" in your circuit.

The 8-digit 7-segment display modules are simple enough that if you opt not to scroll the text then they have the
least-compelling argument for a FILE stream.
Indeed, I used to have students produce the bit vectors for hexadecimal digits on 7-segment displays as a warmup
exercise.
That mastered, requiring that a program display text was more an exercise in tedium than learning anything new.
Bypassing that tedium is why you would use this library with these display modules.
(That, and perhaps you want scrolling text.)

## Discussion

### Scrolling vs Non-Scrolling

If you need a steady display, such as for a calculator, then you should use the non-scrolling option.
In C++, use the `cowpi_configure_seven_segment_display()` function to generate the appropriate `cowpi_display_module_t`
variable.
In C, use `(cowpi_display_module_t) {.display_module = SEVEN_SEGMENT}` without specifying the words-per-minute.

If the message can be transient, and if you need more than eight characters, then you should use the scrolling option.
In C++, use the `cowpi_configure_scrolling_seven_segment_display(_words_per_minute_)` function to generate the
appropriate `cowpi_display_module_t` variable.
In C, use `(cowpi_display_module_t) {.display_module = SEVEN_SEGMENT, .words_per_minute = _words_per_minute_}`.
The scroll rate is expressed as an approximate number of words per minute that should pass along the display.

### Font

While many characters can be displayed recognizably, if imperfectly, on a 7-segment display, others leave much to be
desired.
When you run the example program, you will notice that the 'W' in "Hello World" bears almost no resemblance to the Latin
letter.

### Communication Protocol

The MAX7219 natively supports SPI.
(Technically, there is a small deviation, but not one that you'll notice.)
At a minimum, you need to specify the pin used to indicate which peripheral should latch the data in.
In C++, use the `cowpi_configure_spi(_select_pin_)` function to generate the
appropriate `cowpi_display_module_protocol_t`
variable.
In C, use `(cowpi_display_module_protocol_t) {.protocol = SPI, .select_pin = _select_pin_}`.
By default, the data pin and the clock pin are those used by the SPI hardware, even if you use the library's bit-banged
SPI implementation.
If you wish to use other pins, then specify them with `cowpi_configure_spi(_select_pin_, _data_pin_, _clock_pin_)` in
C++, or
`(cowpi_display_module_protocol_t) {.protocol = SPI, .data_pin = _data_pin_, .clock_pin = _clock_pin_, .select_pin = _select_pin_}`
in C.

## Example

### max7219_seven_segment

The *max7219_seven_segment* example demonstrates both the scrolling and non-scrolling option.
In general, we strongly advise against having multiple FILE streams pointing to the same display module, as the
resulting behavior would be undefined and would unlikely be what you intended.
In this particular case, we get away with it by waiting for the scrolling display to finish before using the
non-scrolling FILE stream.

```c++
FILE *no_scroll_display, *scrolling_display;

void setup(void) {
    // The C++ approach
    no_scroll_display = cowpi_add_display_module(
            cowpi_configure_seven_segment_display(),
            cowpi_configure_spi(SS, MOSI, SCK)
    );
    scrolling_display = cowpi_add_display_module(
            cowpi_configure_scrolling_seven_segment_display(30),
            cowpi_configure_spi(SS, MOSI, SCK)
    );
    
    // The C approach
    /*
    no_scroll_display = cowpi_add_display_module(
            (cowpi_display_module_t) {
                    .display_module = SEVEN_SEGMENT
            },
            (cowpi_display_module_protocol_t) {
                    .protocol = SPI,
                    .data_pin = MOSI,
                    .clock_pin = SCK,
                    .select_pin = SS
            }
    );
    scrolling_display = cowpi_add_display_module(
            (cowpi_display_module_t) {
                    .display_module = SEVEN_SEGMENT,
                    .words_per_minute = 30
            },
            (cowpi_display_module_protocol_t) {
                    .protocol = SPI,
                    .data_pin = MOSI,
                    .clock_pin = SCK,
                    .select_pin = SS
            }
    );
    */
    
    fprintf(scrolling_display, "Hello World!\n");
    // normally, using two file streams on the same display module can have
    // undesired effects, but in this simple example, we should be okay if
    // we wait until the scrolling text is gone
    delay(8000);
}

void loop(void) {
    // On AVR architectures, you can use `fprintf_P` with `PSTR` to put the
    // format string in flash memory, if you want to
#ifdef __AVR__
    fprintf_P(no_scroll_display, PSTR("Hello\n"));
#else
    fprintf(no_scroll_display, "Hello\n");
#endif //__AVR__
    delay(750);
    fprintf(no_scroll_display, "%8s\n", "World!");
    delay(750);
}
```
