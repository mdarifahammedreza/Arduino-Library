/******************************************************************
  @file       readSettings.ino
  @brief      Read the MPU6050 IMU current settings
  @author     David Such
  @copyright  Please see the accompanying LICENSE file

   Code:        David Such
   Version:     1.0.0
   Date:        31/07/23
 
   1.0.0    Original Release            31/07/23

   Credit:  This Library is a fork of Arduino-MPU6050
            ref: https://github.com/jarzebski/Arduino-MPU6050/tree/dev
            Author: Korneliusz Jarzębski

******************************************************************/

#include <ReefwingMPU6050.h>

ReefwingMPU6050 imu;

void setup() {
  // Initialise the LSM9DS1 IMU
  imu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);

  if (imu.connected()) {
    Serial.println("MPU6050 IMU Connected."); 
    imu.calibrateGyro();
  } else {
    Serial.println("MPU6050 IMU Not Detected.");
    while(1);
  }
  
  Serial.print("\n * Sleep Mode:            ");
  Serial.println(imu.getSleepEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Motion Interrupt:      ");
  Serial.println(imu.getIntMotionEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Zero Motion Interrupt:     ");
  Serial.println(imu.getIntZeroMotionEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Free Fall Interrupt:       ");
  Serial.println(imu.getIntFreeFallEnabled() ? "Enabled" : "Disabled");
  
  Serial.print(" * Motion Threshold:          ");
  Serial.println(imu.getMotionDetectionThreshold());

  Serial.print(" * Motion Duration:           ");
  Serial.println(imu.getMotionDetectionDuration());

  Serial.print(" * Zero Motion Threshold:     ");
  Serial.println(imu.getZeroMotionDetectionThreshold());

  Serial.print(" * Zero Motion Duration:      ");
  Serial.println(imu.getZeroMotionDetectionDuration());
  
  Serial.print(" * Clock Source:          ");
  switch(imu.getClockSource()) {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:         ");
  switch(imu.getRange()) {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Gyroscope:         ");
  switch(imu.getScale()) {
    case MPU6050_SCALE_2000DPS:        Serial.println("2000 dps"); break;
    case MPU6050_SCALE_1000DPS:        Serial.println("1000 dps"); break;
    case MPU6050_SCALE_500DPS:         Serial.println("500 dps"); break;
    case MPU6050_SCALE_250DPS:         Serial.println("250 dps"); break;
  } 

  Serial.print(" * Accelerometer Offsets: ");
  Serial.print(imu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(imu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(imu.getAccelOffsetZ());

  Serial.print(" * Gyroscope Offsets: ");
  Serial.print(imu.getGyroOffsetX());
  Serial.print(" / ");
  Serial.print(imu.getGyroOffsetY());
  Serial.print(" / ");
  Serial.println(imu.getGyroOffsetZ());

  Serial.print(" * Accelerometer power delay: ");
  switch(imu.getAccelPowerOnDelay()) {
    case MPU6050_DELAY_3MS:            Serial.println("3 ms"); break;
    case MPU6050_DELAY_2MS:            Serial.println("2 ms"); break;
    case MPU6050_DELAY_1MS:            Serial.println("1 ms"); break;
    case MPU6050_NO_DELAY:             Serial.println("0 ms"); break;
  }  
}

void loop() { }