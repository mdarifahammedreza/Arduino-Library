#ifndef _MPU6050IMU_H
#define _MPU6050IMU_H

#include "Arduino.h"
#include "MPU6050reg.h"

struct Div_Sample_Rate {
  uint8_t divider;
};

typedef enum
{
  disable = 0b000,
  TEMP_OUT_Low = 0b001,
  GYRO_XOUT_Low= 0b010,
  GYRO_YOUT_Low= 0b011,
  GYRO_ZOUT_Low= 0b100,
  ACCEL_XOUT_Low= 0b101,
  ACCEL_YOUT_Low = 0b110,
  ACCEL_ZOUT_Low = 0b111
 
}Fsync_bit_t;

typedef enum
{
  Bandwidth_260 = 0b000,
  Bandwidth_184 = 0b001,
  Bandwidth_94 = 0b010,
  Bandwidth_44 = 0b011,
  Bandwidth_21 = 0b100,
  Bandwidth_10 = 0b101,
  Bandwidth_5 = 0b110,
  Bandwidth_Reserved = 0b111
  
}Dlpf_Config;

typedef enum
{
  range_2g = 0b00,
  range_4g = 0b01,
  range_8g = 0b10,
  range_16g= 0b11
  
}Accel_range;

typedef enum {

  range_250DPS = 0x00,
  range_500DPS = 0x01,
  range_1000DPS= 0x10,
  range_2000DPS= 0x11  
}Gyro_range;

struct FIFO_EN_REG{
  bool TEMP_FIFO_EN = true;
  bool XG_FIFO_EN = true;
  bool YG_FIFO_EN = true;
  bool ZG_FIFO_EN = true;
  bool ACCEL_FIFO_EN = true;
  bool SLV2_FIFO_EN = false;
  bool SLV1_FIFO_EN = false;
  bool SLV0_FIFO_EN = false;
};
  
typedef enum{
  clk_internal = 0b000,
  pll_x_gyro = 0b001,
  pll_y_gyro = 0b010,
  pll_z_gyro = 0b011,
  pll_external_32 = 0b100,
  pll_external_19 = 0b101,
  reserved = 0b110,
  none = 0b111
}clock_source; 

struct lsb_sensitivity
{
  float range_2g = 16384.00, range_4g=8192.00, range_8g=4096.00, range_16g = 2048.00;  
};

struct lsb_gyro_sensitivity
{
  float range_250 = 131.00, range_500 = 65.4, range_1000=32.8, range_2000=16.4;   
};
  
class MPU6050IMU{
	
	bool status;
	
	public:
	
	MPU6050IMU();
	~MPU6050IMU();
	
	uint8_t Who_Am_I();
	bool begin();
	byte readbyte(byte reg);
	void writebyte(byte reg, uint8_t val);
  	void readi2cbytes(uint8_t reg, int count, uint8_t temp[]);
  	void readaccelerometer(float acctemp[], int range);
  	void readgyroscope(float gyrotemp[], int range);

  	void enablefreefall(uint8_t thresold, uint8_t duration);
  	void disablefreefall();
	
	void enablemotiondetection(uint8_t thresold, uint8_t duration);
	void disablemotiondetection();
};



//class MPU6050IMU{};

#endif
