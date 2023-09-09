/******************************************************************
  @file       readTemperature.ino
  @brief      Read the MPU6050 Gyro chip temperature
  @author     David Such
  @copyright  Please see the accompanying LICENSE file

   Code:        David Such
   Version:     1.0.0
   Date:        31/07/23
 
   1.0.0    Original Release            31/07/23

******************************************************************/

#include <ReefwingMPU6050.h>

ReefwingMPU6050 imu;

int loopFrequency = 0;
const long displayPeriod = 1000;
unsigned long previousMillis = 0;
TempData temperature;

void setup() {
  // Initialise the LSM9DS1 IMU
  imu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G);

  //  Start Serial and wait for connection
  Serial.begin(115200);
  while (!Serial);

  if (imu.connected()) {
    Serial.println("MPU6050 IMU Connected."); 
  } else {
    Serial.println("MPU6050 IMU Not Detected.");
    while(1);
  }
}

void loop() {
  //  Read chip temperature
  if (imu.dataAvailable()) {
    temperature = imu.readTemperature();
  }

  //  Display sensor data every displayPeriod, non-blocking.
  if (millis() - previousMillis >= displayPeriod) {
      
    Serial.print("Temperature: ");
    Serial.print(temperature.celsius);
    Serial.print(" °C");
    
    Serial.print("\tLoop Frequency: ");
    Serial.print(loopFrequency);
    Serial.println(" Hz");

    loopFrequency = 0;
    previousMillis = millis();
  }

  loopFrequency++;
}
