/******************************************************************
  @file       gyroRollPitchYaw.ino
  @brief      Simple Gyro based AHRS
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

// Timers
unsigned long timer = 0;
float timeStep = 0.01;

// Pitch, Roll and Yaw values
float pitch = 0;
float roll = 0;
float yaw = 0;

void setup() {
  // Initialise the MPU6050 IMU
  imu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);

  if (imu.connected()) {
    Serial.println("MPU6050 IMU Connected."); 

    //  Set sensitivity threshold (default) and calibrate
    imu.setThreshold(3);
    imu.calibrateGyro();
    delay(20);

    //  Flush the first reading - this is important!
    //  Particularly after changing the configuration.
    imu.readRawGyro();
  } else {
    Serial.println("MPU6050 IMU Not Detected.");
    while(1);
  }
}

void loop() {
  timer = millis();

  //    Read normalized values
  ScaledData norm = imu.readNormalizeGyro();

  //    Calculate Pitch, Roll and Yaw
  //    Need to integrate gyro rates (DPS) to get Degrees
  pitch = pitch + norm.sy * timeStep;
  roll = roll + norm.sx * timeStep;
  yaw = yaw + norm.sz * timeStep;

  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print("\tPitch: ");
  Serial.print(pitch);
  Serial.print("\tYaw: ");
  Serial.print(yaw);
  Serial.println(" Degrees");

  //    Wait for full timeStep period, blocking
  delay((timeStep * 1000) - (millis() - timer));
}