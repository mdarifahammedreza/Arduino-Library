# Arduino_GroveI2C_Ultrasonic
Arduino library for I2C ultrasonic sensor.

Compatible components list:
- [M5stack Grove I2C ultrasonic sensor](https://docs.m5stack.com/zh_CN/unit/sonic.i2c)


## Usage

- `Arduino_GroveI2C_Ultrasonic()`: class constructor
- `begin()`: sensor starts
- `update()`: retrieve data from sensor
- `getDistance()`: get millimeters from sensor
- `getMeters()`: get meters from sensor
- `getTimeTravel()`: get microseconds as time elapsed from ping to echo
- `checkConnection()`: return true if the sensor is connected

## License
Check [license file](./LICENSE).