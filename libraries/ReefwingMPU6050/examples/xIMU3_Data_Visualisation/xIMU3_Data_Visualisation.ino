/******************************************************************
  @file       xIMU3_Data_Visualisation.ino
  @brief      Real-time data from MPU6500 IMU, displayed on the x-IMU3 GUI.
  @author     David Such
  @copyright  Please see the accompanying LICENSE file

  Code:        David Such
  Version:     1.0.0
  Date:        13/04/23

  1.0.0     Original Release.       13/04/23

  Dependency - Requires that the Reefwing_xIMU3_GUI Library is also
               installed. 

******************************************************************/

#include <Reefwing_xIMU3.h>
#include <ReefwingMPU6050.h>

Reefwing_xIMU3 rx;
ReefwingMPU6050 imu;

void setup() {
  // Initialise the MPU6050 IMU
  imu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);

  if (imu.connected()) {
    rx.sendNotification("MPU6050 IMU Connected."); 

    rx.sendNotification("Calibrating IMU - no movement please!");
    imu.calibrateGyro();
    delay(20);

    //  Flush the first reading - this is important!
    //  Particularly after changing the configuration.
    imu.readRawGyro();
    imu.readRawAccel();
    rx.sendNotification("IMU Calibrated.");
  } else {
    rx.sendNotification("MPU6050 IMU Not Detected.");
    while(1);
  }
}

void loop() {
  if (imu.dataAvailable()) {
    //  Read IMU Sensor Data in xIMU3 format
    InertialMessage msg = imu.getInertial();
    TempData td = imu.readTemperature();

    //  Send data messages to xIMU3 GUI
    rx.sendInertial(msg);
    rx.sendTemperature(td);
  }
}
