#include "MPU6050reg.h"
#include <Wire.h>
#include "MPU6050IMU.h"

MPU6050IMU imu;

void setup() {
  // put your setup code here, to run once:
  Wire.begin(8,7);
  Serial.begin(9600);  
  imu.begin();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:

  float temp[3];
  int range = 8;
  
  Serial.println("New measurement");
  Serial.println();

  imu.readaccelerometer(temp, 8);
  
  Serial.print("ax ");
  Serial.println((temp[0]));

  Serial.print("ay ");
  Serial.println((temp[1]));

  Serial.print("az ");
  Serial.println((temp[2]));
  
  delay(100);

  float gyro[3];

  imu.readgyroscope(gyro, 2000);

  Serial.print("gx ");
  Serial.println((gyro[0]));

  Serial.print("gy ");
  Serial.println((gyro[1]));

  Serial.print("gz ");
  Serial.println((gyro[2]));
  Serial.println();
  delay(1000); 
  
}
