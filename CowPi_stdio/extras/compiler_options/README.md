# Enabling Floating Point Conversions on AVR Architectures

By default, `printf` will not perform floating point conversions;
that is, the format specifier `%f` will result in `?` being printed.
(The same is true of `sprintf`.)
If you need floating point conversions, then you can change which implementation the linker will use, by introducing the compiler arguments

```
-Wl,-u,vfprintf -lprintf_flt -lm
```

## If you are using the Arduino IDE

You can introduce the compiler arguments by placing the "extra flags" in a `platform.local.txt` file in the same directory that `platform.txt` is located in.

- See https://support.arduino.cc/hc/en-us/articles/4415103213714-Find-sketches-libraries-board-cores-and-other-files-on-your-computer
- Linux
  - /home/{username}/.arduino15/packages/arduino/hardware/avr/{version_number}/
  - /home/{username}/.arduino15/packages/arduino/hardware/megaavr/{version_number}/
- Mac
  - /Users/{username}/Library/Arduino15/packages/arduino/hardware/avr/{version_number}/
  - /Users/{username}/Library/Arduino15/packages/arduino/hardware/megaavr/{version_number}/
- Windows
  - C:\Users\{username}\AppData\Local\Arduino15\packages\arduino\hardware\avr\{version_number}\
  - C:\Users\{username}\AppData\Local\Arduino15\packages\arduino\hardware\megaavr\{version_number}\

The `platform.local.txt` file provided here can be used.

## If you are using PlatformIO

- You can introduce the compiler arguments on a project-by-project basis by placing the "build flags" in the project's `platformio.ini` file. The `platformio.ini` file provided here demonstrates this.
