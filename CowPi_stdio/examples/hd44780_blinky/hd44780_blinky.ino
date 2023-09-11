#include <CowPi_stdio.h>

// Arduino.h is implicitly included in the Arduino IDE but not in PlatformIO
#include <Arduino.h>

#include <stdbool.h>
#include <stdint.h>

#ifdef __MBED__
#define SDA PIN_WIRE_SDA
#define SCL PIN_WIRE_SCL
#endif

FILE *display;

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

    display = cowpi_add_display_module((cowpi_display_module_t) {.display_module = LCD_CHARACTER}, pins);
    if (!display) {
        printf("received NULL file pointer\n");
        while (1) {}
    } else {
        printf("You should see the backlight blinking.\n");
        printf("Adjust the contrast until you can see the \"Hello, world!\" message.\n");
        fprintf(display, "Hello, world!\n");
    }

    uint8_t blinky[][8] = {
            {0x0E, 0x1F, 0x1D, 0x1F, 0x1F, 0x1F, 0x1F, 0x15},
            {0x0E, 0x1F, 0x1D, 0x1F, 0x1F, 0x1F, 0x1F, 0x0A},
            {0x0E, 0x1F, 0x17, 0x1F, 0x1F, 0x1F, 0x1F, 0x15},
            {0x0E, 0x1F, 0x17, 0x1F, 0x1F, 0x1F, 0x1F, 0x0A},
    };
    cowpi_hd44780_create_character(&pins, 1, blinky[0]);
    cowpi_hd44780_create_character(&pins, 2, blinky[1]);
    cowpi_hd44780_create_character(&pins, 3, blinky[2]);
    cowpi_hd44780_create_character(&pins, 4, blinky[3]);
}

void loop(void) {
    unsigned long time = millis();
    if (time & 0x400) {
        cowpi_sleep_display(display);
    } else {
        cowpi_wake_display(display);
    }
    static enum {
        WAITING_LEFT, MOVING_LEFT, WAITING_RIGHT, MOVING_RIGHT
    } mode = WAITING_RIGHT;
    static int8_t column = 0;
    static int8_t blinky_frame = 1;
    static bool alternate = false;
    if (alternate ^ !(time & 0x100)) {
        alternate = !alternate;
        blinky_frame += alternate ? 1 : -1;
        switch (mode) {
            case WAITING_LEFT:
                fprintf(display, "%13c\r", blinky_frame);
                if (column == 20) {
                    mode = MOVING_LEFT;
                    column = 9;
                } else {
                    column++;
                }
                break;
            case WAITING_RIGHT:
                fprintf(display, "%4c \r", blinky_frame);
                if (column == 20) {
                    mode = MOVING_RIGHT;
                    column = 0;
                } else {
                    column++;
                }
                break;
            case MOVING_LEFT:
#ifdef __AVR__
                // gotta do it this way since AVR doesn't do variable-width conversions
                for (int i = 0; i < column; i++) {
                    fprintf(display, " ");
                }
                fprintf(display, "%4c \r", blinky_frame);
#else
                fprintf(display, "%*c \r", column + 4, blinky_frame);
#endif //__AVR__
                if (column == 0) {
                    mode = WAITING_RIGHT;
                    column = 0;
                    blinky_frame = 1;
                    alternate = false;
                } else {
                    column--;
                }
                break;
            case MOVING_RIGHT:
#ifdef __AVR__
                for (int i = 0; i < column; i++) {
                    fprintf(display, " ");
                }
                fprintf(display, "%4c\r", blinky_frame);
#else
                fprintf(display, "%*c\r", column + 4, blinky_frame);
#endif //__AVR__
                if (column == 9) {
                    mode = WAITING_LEFT;
                    column = 0;
                    blinky_frame = 3;
                    alternate = false;
                } else {
                    column++;
                }
                break;
            default:
                printf("Surprising mode: %d\n", mode);
        }
    }
}
