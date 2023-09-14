/******************************************************************
  @file       freeFallDetection.ino
  @brief      Demonstrate the MPU6050 free fall detection function
  @author     David Such
  @copyright  Please see the accompanying LICENSE file

   Code:        David Such
   Version:     1.0.0
   Date:        31/07/23
 
   1.0.0    Original Release            31/07/23

   Credit:  This Library is a fork of Arduino-MPU6050
            ref: https://github.com/jarzebski/Arduino-MPU6050/tree/dev
            Author: Korneliusz Jarzębski

    Notes:  

    1. If a volatile variable is bigger than a byte (e.g. a 16 bit 
    int or a 32 bit long), then the microcontroller can not read it in one 
    step, if it is an 8 bit microcontroller (e.g., ATMega328P). This means 
    that while your main code section (e.g. your loop) reads the first 8 
    bits of the variable, the interrupt might already change the second 8 
    bits. This will produce random values for the variable.

    2. This sketch assumes that the MPU6050 INT pin is connected to D2 on
    the Arduino UNO or Nano (Interrupt 0).

******************************************************************/

#include <ReefwingMPU6050.h>

ReefwingMPU6050 imu;

const byte interruptPin = 2;

bool ledState = false;
volatile bool freefallDetected = false;
volatile uint8_t freefallBlinkCount = 0;

void setup() {
  // Initialise the LSM9DS1 IMU
  imu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G);

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

  imu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);
  imu.setIntFreeFallEnabled(true);
  imu.setIntZeroMotionEnabled(false);
  imu.setIntMotionEnabled(false);
  imu.setDHPFMode(MPU6050_DHPF_5HZ);
  imu.setFreeFallDetectionThreshold(17);
  imu.setFreeFallDetectionDuration(2);	
  
  checkSettings();

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  attachInterrupt(digitalPinToInterrupt(interruptPin), handleInterrupt, RISING);
}

void handleInterrupt() {
  freefallBlinkCount = 0;
  freefallDetected = true;  
}

void checkSettings() {
  Serial.println();
  Serial.print(" * Sleep Mode:                ");
  Serial.println(imu.getSleepEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Motion Interrupt:     ");
  Serial.println(imu.getIntMotionEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Zero Motion Interrupt:     ");
  Serial.println(imu.getIntZeroMotionEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Free Fall Interrupt:       ");
  Serial.println(imu.getIntFreeFallEnabled() ? "Enabled" : "Disabled");

  Serial.print(" * Free Fal Threshold:          ");
  Serial.println(imu.getFreeFallDetectionThreshold());

  Serial.print(" * Free FallDuration:           ");
  Serial.println(imu.getFreeFallDetectionDuration());
  
  Serial.print(" * Clock Source:              ");
  switch(imu.getClockSource())
  {
    case MPU6050_CLOCK_KEEP_RESET:     Serial.println("Stops the clock and keeps the timing generator in reset"); break;
    case MPU6050_CLOCK_EXTERNAL_19MHZ: Serial.println("PLL with external 19.2MHz reference"); break;
    case MPU6050_CLOCK_EXTERNAL_32KHZ: Serial.println("PLL with external 32.768kHz reference"); break;
    case MPU6050_CLOCK_PLL_ZGYRO:      Serial.println("PLL with Z axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_YGYRO:      Serial.println("PLL with Y axis gyroscope reference"); break;
    case MPU6050_CLOCK_PLL_XGYRO:      Serial.println("PLL with X axis gyroscope reference"); break;
    case MPU6050_CLOCK_INTERNAL_8MHZ:  Serial.println("Internal 8MHz oscillator"); break;
  }
  
  Serial.print(" * Accelerometer:             ");
  switch(imu.getRange()) {
    case MPU6050_RANGE_16G:            Serial.println("+/- 16 g"); break;
    case MPU6050_RANGE_8G:             Serial.println("+/- 8 g"); break;
    case MPU6050_RANGE_4G:             Serial.println("+/- 4 g"); break;
    case MPU6050_RANGE_2G:             Serial.println("+/- 2 g"); break;
  }  

  Serial.print(" * Accelerometer offsets:     ");
  Serial.print(imu.getAccelOffsetX());
  Serial.print(" / ");
  Serial.print(imu.getAccelOffsetY());
  Serial.print(" / ");
  Serial.println(imu.getAccelOffsetZ());

  Serial.print(" * Accelerometer power delay: ");
  switch(imu.getAccelPowerOnDelay()) {
    case MPU6050_DELAY_3MS:            Serial.println("3ms"); break;
    case MPU6050_DELAY_2MS:            Serial.println("2ms"); break;
    case MPU6050_DELAY_1MS:            Serial.println("1ms"); break;
    case MPU6050_NO_DELAY:             Serial.println("0ms"); break;
  }  
  
  Serial.println();
}

void loop() {
  RawData rawAccel = imu.readRawAccel();
  Activites act = imu.readActivites();

  Serial.print(act.isFreeFall);
  Serial.print("\n");
  
  if (freefallDetected) {
    ledState = !ledState;

    digitalWrite(LED_BUILTIN, ledState);
    freefallBlinkCount++;

    if (freefallBlinkCount == 20) {
      freefallDetected = false;
      ledState = false;
      digitalWrite(LED_BUILTIN, ledState);
    }
  }
  
  delay(100);
}