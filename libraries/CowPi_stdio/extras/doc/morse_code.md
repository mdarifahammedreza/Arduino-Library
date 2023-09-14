# Morse Code on Output Pin

Morse Code is brilliant in its simplicity.
For both wired and wireless remote communication, the simplicity of Morse Code led to its use before the technology for
voice communication was ready.
For displaying output, the simplicity of Morse Code is such that it can be used to communicate diagnostics to the user
even when no other outputs are functioning correctly (and also when no outputs other than LEDs are available).

## Discussion

### Configuring the Display

The only display configuration controlled by software is the sending speed, expressed as an approximate number of words
per minute.
In C++, use `cowpi_configure_morse_code(_words_per_minute_)` to generate the appropriate `cowpi_display_module_t`
variable.
In C, use `(cowpi_display_module_t) {.display_module = MORSE_CODE, .words_per_minute = _words_per_minute_}`.

The simplest use of this display would be to illuminate an LED, such as the built-in LED present on most microcontroller
boards.
The specific properties of the LED may limit the speed at which intelligible Morse Code can be sent.
You can also attach a buzzer to the Morse Code pin;
this buzzer would need to have its own oscillator since this library won't oscillate the output on the Morse Code pin to
drive a passive buzzer.

### Font

The "font" characters are sequences of enumerated values (`DIT`, `DAH`, `CHARACTER_SPACE`, etc.) that are converted into
a sequence of symbols of 1s and 0s with the length of each 1 or 0 determined by the enumerated value.
For example, the letter 'n' is expressed as `[DAH, DIT, END_OF_CHARACTER]` which is converted to `1` for 3 units of
time (the DAH), `0` for 1 unit of time (a pause within a character), `1` for 1 unit of time (the DIT), `0` for 3 units
of time (a pause between characters).

### Communication Protocol

There is no communication protocol involved with the "display module."
A pin is simply toggled between logic-high and -low as appropriate.
In C++, use `cowpi_configure_single_pin(_data_pin_)` to generate the appropriate `cowpi_display_module_protocol_t`
variable.
You can use `(cowpi_display_module_protocol_t) {.protocol = NO_PROTOCOL, .data_pin = LED_BUILTIN}` in C or C++.

## Example

### morse_code

The *morse_code* example demonstrates sending a short message using Morse Code through the built-in LED.
In this example, an arbitrary FILE stream is used;
however, if you choose to use Morse Code for diagnostic outputs, you may wish to assign `stderr` to the FILE stream
returned by `cowpi_add_display_module()` as a matter of convention.

```c++
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
```