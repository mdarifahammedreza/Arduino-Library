/******************************************************************
  @file       accelRollPitch.ino
  @brief      Simple Accelerometer based AHRS
  @author     David Such
  @copyright  Please see the accompanying LICENSE file

  Code:        David Such
  Version:     1.0.0
  Date:        31/07/23
 
  1.0.0        Original Release            31/07/23

  Credit:  This Library is a fork of Arduino-MPU6050
            ref: https://github.com/jarzebski/Arduino-MPU6050/tree/dev
            Author: Korneliusz Jarzębski

******************************************************************/

#include <ReefwingMPU6050.h>

ReefwingMPU6050 imu;

//  Display and Loop Frequency
int loopFrequency = 0;
const long displayPeriod = 1000;
unsigned long previousMillis = 0;

void setup() {
  // Initialise the MPU6050 IMU
  imu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);

  if (imu.connected()) {
    Serial.println("MPU6050 IMU Connected."); 

    imu.calibrateGyro();
    delay(20);

    //  Flush the first reading - this is important!
    //  Particularly after changing the configuration.
    imu.readRawAccel();
  } else {
    Serial.println("MPU6050 IMU Not Detected.");
    while(1);
  }
}

void loop() {
  // Read normalized values 
  ScaledData normAccel = imu.readNormalizeAccel();

  // Calculate Pitch & Roll
  int pitch = -(atan2(normAccel.sx, sqrt(normAccel.sy * normAccel.sy + normAccel.sz * normAccel.sz)) * 180.0) / M_PI;
  int roll = (atan2(normAccel.sy, normAccel.sz) * 180.0) / M_PI;

  if (millis() - previousMillis >= displayPeriod) {
    //  Display sensor data every displayPeriod, non-blocking.
    Serial.print("Roll: ");
    Serial.print(roll);
    Serial.print("\tPitch: ");
    Serial.print(pitch);
    Serial.print(" Degrees");
  
    Serial.print("\tLoop Frequency: ");
    Serial.print(loopFrequency);
    Serial.println(" Hz");

    Serial.print("Accel X: ");
    Serial.print(normAccel.sx);
    Serial.print("\tAccel Y: ");
    Serial.print(normAccel.sy);
    Serial.print("\tAccel Z: ");
    Serial.print(normAccel.sz);
    Serial.println(" G'S");

    loopFrequency = 0;
    previousMillis = millis();
  }

  loopFrequency++;
  delay(10);
}