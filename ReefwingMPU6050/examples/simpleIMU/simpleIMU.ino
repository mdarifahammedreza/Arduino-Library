/******************************************************************
  @file       simpleIMU.ino
  @brief      Display data from all 3 sensors of the MPU6050
  @author     David Such
  @copyright  Please see the accompanying LICENSE file

  Code:        David Such
  Version:     1.0.0
  Date:        31/07/23
 
  1.0.0        Original Release            31/07/23

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
    imu.readRawGyro();
    imu.readRawAccel();
  } else {
    Serial.println("MPU6050 IMU Not Detected.");
    while(1);
  }
}

void loop() {
  imu.updateSensorData();

  if (millis() - previousMillis >= displayPeriod) {
    //  Display sensor data every displayPeriod, non-blocking.
    Serial.print("Gyro X: ");
    Serial.print(imu.data.gx);
    Serial.print("\tGyro Y: ");
    Serial.print(imu.data.gy);
    Serial.print("\tGyro Z: ");
    Serial.print(imu.data.gz);
    Serial.print(" DPS");
  
    Serial.print("\tLoop Frequency: ");
    Serial.print(loopFrequency);
    Serial.println(" Hz");

    Serial.print("Accel X: ");
    Serial.print(imu.data.ax);
    Serial.print("\tAccel Y: ");
    Serial.print(imu.data.ay);
    Serial.print("\tAccel Z: ");
    Serial.print(imu.data.az);
    Serial.println(" G'S");

    Serial.print("Temperature: ");
    Serial.print(imu.gyroTemp.celsius);
    Serial.println(" °C");

    loopFrequency = 0;
    previousMillis = millis();
  }

  loopFrequency++;
}