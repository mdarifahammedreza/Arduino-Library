#ifndef _MPU6050reg_H
#define _MPU6050reg_H

#include "Arduino.h"

#define AddressMPU 		(0x68)

#define WHO_AM_I 		  0x75
#define WHO_AM_I_RES	0x68

//Configuration
#define SAMPLRT_DIV 	0x19
#define CONFIG			  0x1A
#define GYRO_CONFIG		0x1B
#define ACCEL_CONFIG 	0x1C
#define FIFO_EN			  0x23

#define PWR_MGMT_1 		0x6B

//measurement Registers

#define ACCEL_XOUT_H 	  0x3B
#define ACCEL_XOUT_L 	  0x3C
#define ACCEL_YOUT_H    0x3D
#define ACCEL_YOUT_L    0x3E
#define ACCEL_ZOUT_H	  0x3F
#define ACCEL_ZOUT_L    0x40

//Gyroscope Registers

#define GYRO_XOUT_H 	0x43
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L 	0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48

//Temperature Registers
#define TEMP_OUT_H 		0x41
#define TEMP_OUT_L		0x42

//interrupt Registers

#define INT_PIN_CONFIG 0x37
#define INT_ENABLE 0x38
#define INT_STATUS 0x3A

#define MOT_THR 0x1F
#define MOT_DUR 0x20

#define FF_THR 0x1D
#define FF_DUR 0x1E

#endif
