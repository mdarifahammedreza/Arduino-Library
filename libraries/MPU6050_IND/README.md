# MPU6050_IND

Library for MPU6050 6-Axis IMU sensor.
I have created this library for reading the sensor values from MPU-6050 6-Axis Accelerometer plus Gyroscope. Both Accelerometer and Gyro can measure values in all six directions(+/-)*(X,Y,Z).
The rate at which the device can measure this values varies upon configuration of the values of registors internal to the device. Normally it can range from 260Hz(for Acc) and 256Hz(For Gyro) to 3 or 4 Hz for the Accelerometer and Gyroscope measurements. This bandwidth rates are enough for some applications such as fall detection, gesture detection, movement sensor etc. 

There are various ranges available for Accelerometer and Gyroscopes. For accelerometer it is from +/-(2g, 4g, 8g, 16g) and for Gyroscope it is +/-(250*/s, 500*/s, 1000*/s, 2000*/s). 

There are various interrupt support for this sensor. The pin INT can  be used for this purpose. The following are the kinds of interrupts which can be gnerated on this pin.

		1) Free Fall detection
		2) Motion enable interrut
 		3) FIFO overflow
		4) Sensor data ready
		5) I2C master interrupt

In this library will follow the first two interrupt in order detect either fall detection or motion detection at a time. Both can be detected but needs to be distinguished by reading INT_STATUS Register.

## Library Functions

1) mpu.begin()
2) mpu.readaccelerometer()
3) mpu.readgyroscope()
4) mpu.enablefreefall(uint8_t thresold, uint8_t t)

i2c communication specific functions

1) mpu.readbyte()
2) mpu.writebyte()
3) mpu.readi2cbytes(uint8_t reg, int count, uint8_t temp[])
4) mpu.readbyte(byte reg)
5) mpu.writebyte(byte reg, uint8_t val)

## Free-Fall detection

Free-fall detection is the capability of MPU6050 to detect a fall of the sensor. When the readings of accelerometer sensor from all the three axis is less than this value set in by the function #enablefreefall(), the interrupt will get triggered on INT pin of the MPU6050 sensor.

## Roadmap
The roadmap is to create a lightweight library at the same time providing the flexibility to use on different platforms. There are several things which can be done or added to the current project

		1) Self-testing MPU-registers
		2) Adding external I2C sensor support for sensor-fusion
		3) Adding temprature measurements
		4) Kalman-filter or any other techniques to find a speed and trejecroty etc.'
		5) Improving the library for tuning different parameters

## Contributing
Open for contribution from anyone. 
Just clone/fork this repository and create pull requests.

## Authors and acknowledgment
Ujjval Rathod

## License
Free and open source for everyone

## Project status
Active and in development..
