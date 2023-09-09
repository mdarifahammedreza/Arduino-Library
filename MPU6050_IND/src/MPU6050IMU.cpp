#include "Arduino.h"
#include "MPU6050IMU.h"
#include "MPU6050reg.h"

#include <Wire.h>

//class MPU6050IMU{};

MPU6050IMU::MPU6050IMU(void){}

MPU6050IMU::~MPU6050IMU(void){}

uint8_t MPU6050IMU::Who_Am_I(){
	
	uint8_t temp;	
	temp = readbyte(WHO_AM_I);
	if (temp != WHO_AM_I_RES){
		Serial.print("Wrong Device!");
		}
	return temp;
}
	
bool MPU6050IMU::begin(){
   
   uint8_t Powermanagement = (0b000 << 5)|(0b00 << 3)|(pll_x_gyro);  //disable SLEEP mode, enable pll_x gyro clock
   writebyte(PWR_MGMT_1, Powermanagement);                            //turn-on powermanagement register

   FIFO_EN_REG fifoen;
   uint8_t fifoenable = (fifoen.TEMP_FIFO_EN<<7 | fifoen.XG_FIFO_EN<<6 | fifoen.YG_FIFO_EN<<5 | fifoen.ZG_FIFO_EN<<4 | fifoen.ACCEL_FIFO_EN<<3 | 0b000);
   writebyte(FIFO_EN, fifoenable);                                    //enable fifo for various measurements
   
   Div_Sample_Rate sample_rate;
   sample_rate.divider = 0x00;
   writebyte(SAMPLRT_DIV, sample_rate.divider);                       //keep maximum sample rate

   uint8_t accelconfig = range_16g << 3;
   writebyte(ACCEL_CONFIG, accelconfig);                              //turn-on Accelerometer

   uint8_t gyroconfig = range_2000DPS << 3;
   writebyte(GYRO_CONFIG, gyroconfig);                                 //turn-on Gyroscope
   
   return 1;
}

void MPU6050IMU::readaccelerometer(float acctemp[], int range) {

  uint8_t acc[6];
  uint8_t accelconfig;
  int16_t temp[3];
  
  float range_div;
  
  readi2cbytes(ACCEL_XOUT_H, 6, acc);   

  temp[0] = acc[0]<<8 | acc[1];
  temp[1] = acc[2]<<8 | acc[3];
  temp[2] = acc[4]<<8 | acc[5];
  
  lsb_sensitivity sense;  
  

  switch(range){
    case 16 :
	  accelconfig = range_16g << 3;
	  writebyte(ACCEL_CONFIG, accelconfig);
      range_div = sense.range_16g;
      acctemp[0] = (temp[0]/2048.00) * 16;
      acctemp[1] = (temp[1]/2048.00) * 16;
      acctemp[2] = (temp[2]/2048.00) * 16;      
      break;
    case 8  :
      accelconfig = range_8g << 3;
	  writebyte(ACCEL_CONFIG, accelconfig);
      range_div = sense.range_8g;
      acctemp[0] = (temp[0]/range_div) * 8;
      acctemp[1] = (temp[1]/range_div) * 8;
      acctemp[2] = (temp[2]/range_div) * 8;
      break;
    case 4  :
	  accelconfig = range_4g << 3;
	  writebyte(ACCEL_CONFIG, accelconfig);
      range_div = sense.range_4g;      
      acctemp[0] = (temp[0]/sense.range_4g) * 4;
      acctemp[1] = (temp[1]/sense.range_4g) * 4;
      acctemp[2] = (temp[2]/sense.range_4g) * 4;
      break;
    case 2  :
      accelconfig = range_2g << 3;
	  writebyte(ACCEL_CONFIG, accelconfig);
      range_div = sense.range_2g;
      acctemp[0] = (temp[0]/sense.range_2g) * 2;
      acctemp[1] = (temp[1]/sense.range_2g) * 2;
      acctemp[2] = (temp[2]/sense.range_2g) * 2;
      break;
    }
}

void MPU6050IMU::readgyroscope(float gyrotemp[], int range) 
{
  
  uint8_t gyro[6];
  int16_t temp[3];

  lsb_gyro_sensitivity sense;
  
  readi2cbytes(GYRO_XOUT_H, 6, gyro);

  temp[0] = gyro[0]<<8 | gyro[1];
  temp[1] = gyro[2]<<8 | gyro[3];
  temp[2] = gyro[4]<<8 | gyro[5];

  switch(range){
    case 2000 :
      gyrotemp[0] = temp[0]/sense.range_2000;
      gyrotemp[1] = temp[1]/sense.range_2000;
      gyrotemp[2] = temp[2]/sense.range_2000;
      break;
    case 1000 :
      gyrotemp[0] = temp[0]/sense.range_1000;
      gyrotemp[1] = temp[0]/sense.range_1000;
      gyrotemp[2] = temp[0]/sense.range_1000;
      break;
    case 500  :
      gyrotemp[0] = temp[0]/sense.range_500;
      gyrotemp[1] = temp[0]/sense.range_500;
      gyrotemp[2] = temp[0]/sense.range_500;
      break;
    case 250  :
      gyrotemp[0] = temp[0]/sense.range_250;
      gyrotemp[1] = temp[0]/sense.range_250;
      gyrotemp[2] = temp[0]/sense.range_250;
      break;
    
    }  
}

void MPU6050IMU::enablefreefall(uint8_t thresold, uint8_t duration)
{

  writebyte(INT_PIN_CONFIG,0x20);               //for seting active-high interupt cleared by reading INT_STATUS.
  writebyte(INT_ENABLE, 0x80);
  writebyte(FF_THR, thresold);
  writebyte(FF_DUR, duration);
  
}

void MPU6050IMU::disablefreefall(){

  writebyte(INT_ENABLE, 0x80);
  
}  
void MPU6050IMU::enablemotiondetection(uint8_t thresold, uint8_t duration)
{
	writebyte(INT_PIN_CONFIG, 0x20);
	writebyte(INT_ENABLE, 0x20);
	writebyte(MOT_THR, thresold);
	writebyte(MOT_DUR, duration);
	
}

void MPU6050IMU::disablemotiondetection(){
	writebyte(INT_ENABLE, 0x80);
}
byte MPU6050IMU::readbyte(byte reg){
	Wire.beginTransmission(AddressMPU);
	Wire.write(reg);
	Wire.endTransmission();
	Wire.requestFrom(AddressMPU, 1);
	return Wire.read();
}
	
void MPU6050IMU::writebyte(byte reg, uint8_t val) {
	
	Wire.beginTransmission(AddressMPU);
	Wire.write(reg);
	Wire.write(val);
	Wire.endTransmission();
		
}

void MPU6050IMU::readi2cbytes(uint8_t reg, int count, uint8_t temp[]) {
  
  Wire.beginTransmission(AddressMPU);
  Wire.write(reg);
  Wire.endTransmission();
  Wire.requestFrom(AddressMPU, count);

  for(int i=0; i<count; i++){
    temp[i] = Wire.read();    
    }  
  
}
