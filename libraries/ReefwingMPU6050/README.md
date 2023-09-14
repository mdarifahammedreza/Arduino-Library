![version](https://img.shields.io/github/v/tag/Reefwing-Software/Reefwing-MPU6050) ![license](https://img.shields.io/badge/license-MIT-green) ![release](https://img.shields.io/github/release-date/Reefwing-Software/Reefwing-MPU6050?color="red") ![open source](https://badgen.net/badge/open/source/blue?icon=github)

## Reefwing MPU6050 Arduino Library

 The primary additions for the Reefwing IMU libraries is to use the [Reefwing imuTypes](https://github.com/Reefwing-Software/Reefwing-imuTypes) library header to make it compatible with the [Reefwing AHRS Library](https://github.com/Reefwing-Software/Reefwing-AHRS) and allow easy graphing of IMU values via the [Reefwing xIMU3](https://github.com/Reefwing-Software/Reefwing-xIMU3) data visualisation library. The second addition is achieved by providing a time stamp for each sensor reading. While we are going to the trouble of creating our own versions, we also add in some additional functionality, not found in the standard libraries.

## The MPU-6050

The MPU-6050 consists of a 3-axis gyroscope and 3-axis accelerometer. The sensor ranges are:

- Gyroscope: + 250, 500, 1000, 2000 ° / s

- Accelerometer: ± 2, ± 4, ± 8, ± 16 g

As usual our first step is to have a look at the MPU-6050 data sheet. There we made the discovery that:

`The MPU-6000 and MPU-6050 are identical, except that the MPU-6050 supports the I²C serial interface only, and has a separate VLOGIC reference pin. The MPU-6000 supports both I²C and SPI interfaces and has a single supply pin, VDD, which is both the device's logic reference supply and the analog supply for the part.`

This is great news since we have already written a library for the [MPU-6000](https://github.com/Reefwing-Software/MPU6x00)! The bad news is that our Library only currently supports SPI, since that was all we needed. To handle the MPU-6050, we will need to add support for I²C. 

Instead of just adding I²C support to our MPU-6x00 Library, we decided to fork another library which provided a lot of additional functionality. This library is the [Arduino-MPU6050](https://github.com/jarzebski/Arduino-MPU6050/tree/dev) written by Korneliusz Jarzębski.

## The GY-521 Development Board

The MPU-6050 that we are using is mounted on a GY-521 Development Board. The GY-521, specifications are:

- MPU-6050 IMU
- 3V3 LDO Voltage Regulator
- Power LED
- Bypass Capacitors
- I²C Pull-up Resistors
- Header Pins
- Two I²C addresses: Default `0x68`, change to `0x69` by connecting the ADR pin to 3.3V
- Power supply: 3V-5V
- Logic Level: 3.3V

One concern that we had was that the Arduino Nano (5V logic) had its SDA and SCL pins connected directly to the corresponding pins on the MPU-6050 (3V3 logic), with no voltage level conversion. To determine if this is an issue we need to check a couple of things:

1. Will the I²C HIGH signal from the MPU-6050 be above the minimum high level threshold of the ATMega328P?

2. Does the ATMega328P drive its I²C pins (A4 and A5) to 5V during I²C communication? If it did we would be shorting 5V to the 3V3 power bus via the pull-up resistors, which could exceed the current capacity of the 328P pins (20 mA), if the pull-up resistor value was too low. It could also exceed the maximum pin voltage of the MPU-6050 and either kill the IMU or shorten its life.

Looking firstly at point 1, in the I²C Bus Specification, logic `0` (`LOW`) and `1` (`HIGH`) are not fixed but depend on VDD (3V3 in the GY-521). Input reference levels are set as 30 % and 70 % of VDD (i.e. for the MPU-6050, VIL = 0.3 x VDD = 0.99 V and VIH = 0.7 x VDD = 2.31 V).

According to Table 28–7, the 2-wire Serial (I²C) Bus Requirements in the ATMega328P Data Sheet are:

- VIL needs to be between -0.5 and 0.3 x Vcc = 1.5 V. **No problem.**

- VIH needs to be between 0.7 x Vcc = 3.5 and Vcc + 0.5 = 5.5 V. This could be a problem as the MPU-6050 outputs a HIGH of between 2.31 V and 3.3 V, **which is below the specified minimum threshold** of 3.5 V. The correct solution is to use a voltage converter on the I²C bus between the Nano and the MPU-6050, or use an Arduino with 3V3 logic.

Moving to point 2, the ATMega328P port pins have three states (tri-state):

- OUTPUT `HIGH` (5 V)
- OUTPUT `LOW` (GND)
- INPUT `Hi-Z` (Floating - High Impedance)

If a pin is in a high impedance state, it can be pulled `HIGH` or `LOW` by an external device, like the MPU-6050, without much current flow. When an input is `Hi-Z`, it allows an external device to control its logic level.

Note the two 4K7 pull-up resistors on the GY-521. The I²C bus protocol requires the use of pull-up resistors on the communication lines. Devices on the bus become inactive by releasing the lines and tri-stating their outputs, leaving only the pull-up resistors to pull the lines high. When a device wants to communicate, it exits the high impendance (`Hi-Z`) state and drives the line low. This prevents bus contention, where conflicting devices attempt to drive the line to different states.

There are no external pull-ups on the SDA and SCL pins of the Arduino Nano. There is an internal pull-up available on the ATMega328P I/O pins, which we **DON'T** want to enable. Thus when our I²C bus is not being driven `LOW` by either the Nano or IMU, it will sit at 3V3. **No problem.**

## The Reefwing MPU-6050 Library

Our library is a fork of the [Arduino-MPU6050](https://github.com/jarzebski/Arduino-MPU6050/tree/dev) library written by Korneliusz Jarzębski, with some new methods:

- `bool connected()` - indicates that the MPU-6050 has been detected;
- `bool dataAvailable()` - new IMU data is available to be read;
- `void updateSensorData()` - updates the public SensorData struct (called data);
- `InertialMessage getInertial()` - returns IMU data suitable for transmission to the xIMU3 Data Visualisation application;

In addition to these methods we have also modified the library to return data in the imuTypes format (e.g., `RawData`, `ScaledData`, and `TempData`). These data types include a time stamp indicating when the sensor reading was taken.

The best way to understand how to use the Reefwing library is to look at the sketches in the examples folder:

- `readSettings.ino` - displays the the MPU6050 IMU current settings on the Arduino IDE Serial Monitor.
- `readTemperature.ino` - reads and displays the MPU6050 Gyro chip temperature on the Arduino IDE Serial Monitor.
- `simpleIMU.ino` - displays data from all 3 sensors of the MPU6050 on the Arduino IDE Serial Monitor.
- `accelRollPitch.ino` - simple accelerometer based AHRS displayed on the Arduino IDE Serial Monitor.
- `gyroRollPitchYaw.ino` - simple gyroscope based AHRS displayed on the Arduino IDE Serial Monitor.
- `freeFallDetection.ino` - demonstrates the MPU6050 free fall detection function. Requires connection of the interrupt pin.
- `xIMU3_Data_Visualisation.ino` - real-time data from MPU6500 IMU, streamed and displayed on the [x-IMU3 GUI](https://x-io.co.uk/x-imu3/#downloads) application.

## Dependencies

This library needs the [Reefwing_imuTypes](https://github.com/Reefwing-Software/Reefwing-imuTypes) library installed as well. If you want to run the xIMU3_Data_Visualisation sketch, you will also need to install the [Reefwing_xIMU3](https://github.com/Reefwing-Software/Reefwing-xIMU3) library.