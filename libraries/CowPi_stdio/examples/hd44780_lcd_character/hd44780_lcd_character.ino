#include <CowPi_stdio.h>

// Arduino.h is implicitly included in the Arduino IDE but not in PlatformIO
#include <Arduino.h>

#ifdef __MBED__
#define SDA PIN_WIRE_SDA
#define SCL PIN_WIRE_SCL
#endif

#include <stdbool.h>
#include <stdint.h>

FILE *display;
const uint8_t backslash = 0x01;

void fill_display(void);

void setup(void) {
    cowpi_stdio_setup(9600);

    // **********
    // CHOOSE SERIAL-TO-PARALLEL MAPPING (typically can omit adapter mapping if using COWPI_DEFAULT)
    enum adapter_mappings adapter_mapping = COWPI_DEFAULT;
    // enum adapter_mappings adapter_mapping = ADAFRUIT;
    // **********

    // **********
    // CHOOSE SPI OR I2C
    // cowpi_display_module_protocol_t pins = cowpi_configure_spi(COWPI_DEFAULT_SPI_SELECT_PIN,
    //                                                            COWPI_DEFAULT_SPI_DATA_PIN,
    //                                                            COWPI_DEFAULT_SPI_CLOCK_PIN,
    //                                                            adapter_mapping);
    cowpi_display_module_protocol_t pins = cowpi_configure_i2c(cowpi_discover_i2c_address(COWPI_DEFAULT_I2C_DATA_PIN,
                                                                                          COWPI_DEFAULT_I2C_CLOCK_PIN),
                                                               COWPI_DEFAULT_I2C_DATA_PIN,
                                                               COWPI_DEFAULT_I2C_CLOCK_PIN,
                                                               adapter_mapping);
    // **********

    display = cowpi_add_display_module(
            (cowpi_display_module_t) {
                    .display_module = LCD_CHARACTER,
                //    .width = 20,
                //    .height = 4
            },
            pins
    );
    if (!display) {
        printf("received NULL file pointer\n");
        while (1) {}
    }

    // ROM Code A00 has a Yen symbol instead of '\\'
    uint8_t backslash_matrix[8];
    cowpi_font_ascii_to_5wide_dotmatrix(backslash_matrix, '\\');
    cowpi_hd44780_create_character(&pins, backslash, backslash_matrix);

    cowpi_hd44780_clear_display(&pins);
    // On AVR architectures, you can use `fprintf_P` with `PSTR` to put the
    // format string in flash memory, if you want to
    // !!! Notice how much RAM the text in this example uses !!!
#ifdef __AVR__
    fprintf_P(display, PSTR("Hello, World!\n"));
#else
    fprintf(display, "Hello, World!\n");
#endif //__AVR__
    delay(1000);
}

void loop(void) {
    fill_display();
    fprintf(display, "Most characters\n");
    fprintf(display, "overwrite others\n");
    fill_display();
    printf("Horizontal tab (\\t, 0x09) advances 1 space without overwriting the character.\n");
    fprintf(display, "Advance\tcursor\n");
    // place a custom character with the literal byte for its CGRAM address
    fprintf(display, "\t\twith\t\x1t\n");
    fill_display();
    printf("Form feed, or new page, (\\f, 0x0C) places the cursor in the top row and performs a carriage return.\n");
    fprintf(display, "return home\n");
    // or place a custom character with the "%c" conversion specifier
    fprintf(display, "  with %cf\f", (char) backslash);
    delay(1500);
    fprintf(display, "like this\n");
    fill_display();
    printf("Vertical tab (\\v, 0xB) advances to the next row without clearing any characters, and performs a carriage return\n");
    fprintf(display, "Go to next row\v");
    fprintf(display, "  with %cv\v", (char) backslash);
    fill_display();
    printf("Line feed, or new line, (\\n, 0xA) clears the remaining characters, advances to the next row, and performs a carriage return\n");
    fprintf(display, "Clear row & go\n");
    fprintf(display, "to next row: %cn\n", (char) backslash);
    fill_display();
    printf("Carriage return (\\r, 0x0D) places the cursor at the left position of the current row.\n");
    fprintf(display, "Go to row start\n");
    fprintf(display, "  with %cr  \r", (char) backslash);
    delay(1500);
    fprintf(display, "like this\n");
    fill_display();
    fprintf(display, "A too-long string advances row.\n");
    fill_display();
    fprintf(display, "\f\v\vAdvancing past\nthe bottom\n");
    delay(1500);
    fprintf(display, "goes to the top.\n");
}

void fill_display(void) {
    delay(2000);
    fprintf(display, "\f----------------\n");
    fprintf(display, "----------------\n");
    fprintf(display, "----------------\n");
    fprintf(display, "----------------\f");
    delay(1000);
}
