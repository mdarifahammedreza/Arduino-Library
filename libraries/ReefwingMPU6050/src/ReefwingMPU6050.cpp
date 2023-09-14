/******************************************************************
  @file       ReefwingMPU6050.h
  @brief      Arduino Library for MPU6050 IMU using I2C
  @author     David Such
  @copyright  Please see the accompanying LICENSE file
  
   Version:     1.0.0
   Date:        31/07/23
 
   1.0.0    Original Release            31/07/23

   Credit:  This Library is a fork of Arduino-MPU6050
            ref: https://github.com/jarzebski/Arduino-MPU6050/tree/dev
            Author: Korneliusz Jarzębski
 
 ******************************************************************/

#include <Wire.h>
#include <math.h>

#include "ReefwingMPU6050.h"

/******************************************************************
 *
 * MPU6050 Implementation - 
 * 
 ******************************************************************/

bool ReefwingMPU6050::begin(GyroScale scale, AccelScale range, int mpua) {
    // Set Address
    mpuAddress = mpua;

    Wire.begin();

    // Reset calibrate values
    dg.x = 0;
    dg.y = 0;
    dg.z = 0;
    useCalibrate = false;

    // Reset threshold values
    tg.x = 0;
    tg.y = 0;
    tg.z = 0;
    actualThreshold = 0;

    // Check MPU6050 Who Am I Register
    if (fastRegister8(MPU6050_REG_WHO_AM_I) != 0x68) {
	    return false;
    }

    // Set Clock Source
    setClockSource(ClockSource::MPU6050_CLOCK_PLL_XGYRO);

    // Set Scale & Range
    setScale(scale);
    setRange(range);

    // Disable Sleep Mode
    setSleepEnabled(false);

    return true;
}

bool ReefwingMPU6050::connected()  {
    return (fastRegister8(MPU6050_REG_WHO_AM_I) == mpuAddress);
}

void ReefwingMPU6050::setScale(GyroScale scale) {
    uint8_t value;

    switch (scale) {
        case MPU6050_SCALE_250DPS:
            dpsPerDigit = .007633f;
            break;
        case MPU6050_SCALE_500DPS:
            dpsPerDigit = .015267f;
            break;
        case MPU6050_SCALE_1000DPS:
            dpsPerDigit = .030487f;
            break;
        case MPU6050_SCALE_2000DPS:
            dpsPerDigit = .060975f;
            break;
        default:
            break;
    }

    value = readRegister8(MPU6050_REG_GYRO_CONFIG);
    value &= 0b11100111;
    value |= (scale << 3);
    writeRegister8(MPU6050_REG_GYRO_CONFIG, value);
}

GyroScale ReefwingMPU6050::getScale() {
    uint8_t value;

    value = readRegister8(MPU6050_REG_GYRO_CONFIG);
    value &= 0b00011000;
    value >>= 3;
    return (GyroScale)value;
}

void ReefwingMPU6050::setRange(AccelScale range) {
    uint8_t value;

    switch (range) {
        case MPU6050_RANGE_2G:
            rangePerDigit = .000061f;
            break;
        case MPU6050_RANGE_4G:
            rangePerDigit = .000122f;
            break;
        case MPU6050_RANGE_8G:
            rangePerDigit = .000244f;
            break;
        case MPU6050_RANGE_16G:
            rangePerDigit = .0004882f;
            break;
        default:
            break;
    }

    value = readRegister8(MPU6050_REG_ACCEL_CONFIG);
    value &= 0b11100111;
    value |= (range << 3);
    writeRegister8(MPU6050_REG_ACCEL_CONFIG, value);
}

AccelScale ReefwingMPU6050::getRange() {
    uint8_t value;

    value = readRegister8(MPU6050_REG_ACCEL_CONFIG);
    value &= 0b00011000;
    value >>= 3;
    return (AccelScale)value;
}

void ReefwingMPU6050::setDHPFMode(HighPassFilter dhpf) {
    uint8_t value;

    value = readRegister8(MPU6050_REG_ACCEL_CONFIG);
    value &= 0b11111000;
    value |= dhpf;
    writeRegister8(MPU6050_REG_ACCEL_CONFIG, value);
}

void ReefwingMPU6050::setDLPFMode(LowPassFilter dlpf) {
    uint8_t value;

    value = readRegister8(MPU6050_REG_CONFIG);
    value &= 0b11111000;
    value |= dlpf;
    writeRegister8(MPU6050_REG_CONFIG, value);
}

void ReefwingMPU6050::setClockSource(ClockSource source) {
    uint8_t value;

    value = readRegister8(MPU6050_REG_PWR_MGMT_1);
    value &= 0b11111000;
    value |= source;
    writeRegister8(MPU6050_REG_PWR_MGMT_1, value);
}

ClockSource ReefwingMPU6050::getClockSource() {
    uint8_t value;

    value = readRegister8(MPU6050_REG_PWR_MGMT_1);
    value &= 0b00000111;
    return (ClockSource)value;
}

bool ReefwingMPU6050::getSleepEnabled() {
    return readRegisterBit(MPU6050_REG_PWR_MGMT_1, 6);
}

void ReefwingMPU6050::setSleepEnabled(bool state) {
    writeRegisterBit(MPU6050_REG_PWR_MGMT_1, 6, state);
}

bool ReefwingMPU6050::getIntZeroMotionEnabled() {
    return readRegisterBit(MPU6050_REG_INT_ENABLE, 5);
}

void ReefwingMPU6050::setIntZeroMotionEnabled(bool state) {
    writeRegisterBit(MPU6050_REG_INT_ENABLE, 5, state);
}

bool ReefwingMPU6050::getIntMotionEnabled() {
    return readRegisterBit(MPU6050_REG_INT_ENABLE, 6);
}

void ReefwingMPU6050::setIntMotionEnabled(bool state) {
    writeRegisterBit(MPU6050_REG_INT_ENABLE, 6, state);
}

bool ReefwingMPU6050::getIntFreeFallEnabled() {
    return readRegisterBit(MPU6050_REG_INT_ENABLE, 7);
}

void ReefwingMPU6050::setIntFreeFallEnabled(bool state) {
    writeRegisterBit(MPU6050_REG_INT_ENABLE, 7, state);
}

uint8_t ReefwingMPU6050::getMotionDetectionThreshold() {
    return readRegister8(MPU6050_REG_MOT_THRESHOLD);
}

void ReefwingMPU6050::setMotionDetectionThreshold(uint8_t threshold) {
    writeRegister8(MPU6050_REG_MOT_THRESHOLD, threshold);
}

uint8_t ReefwingMPU6050::getMotionDetectionDuration() {
    return readRegister8(MPU6050_REG_MOT_DURATION);
}

void ReefwingMPU6050::setMotionDetectionDuration(uint8_t duration) {
    writeRegister8(MPU6050_REG_MOT_DURATION, duration);
}

uint8_t ReefwingMPU6050::getZeroMotionDetectionThreshold() {
    return readRegister8(MPU6050_REG_ZMOT_THRESHOLD);
}

void ReefwingMPU6050::setZeroMotionDetectionThreshold(uint8_t threshold) {
    writeRegister8(MPU6050_REG_ZMOT_THRESHOLD, threshold);
}

uint8_t ReefwingMPU6050::getZeroMotionDetectionDuration() {
    return readRegister8(MPU6050_REG_ZMOT_DURATION);
}

void ReefwingMPU6050::setZeroMotionDetectionDuration(uint8_t duration) {
    writeRegister8(MPU6050_REG_ZMOT_DURATION, duration);
}

uint8_t ReefwingMPU6050::getFreeFallDetectionThreshold() {
    return readRegister8(MPU6050_REG_FF_THRESHOLD);
}

void ReefwingMPU6050::setFreeFallDetectionThreshold(uint8_t threshold) {
    writeRegister8(MPU6050_REG_FF_THRESHOLD, threshold);
}

uint8_t ReefwingMPU6050::getFreeFallDetectionDuration() {
    return readRegister8(MPU6050_REG_FF_DURATION);
}

void ReefwingMPU6050::setFreeFallDetectionDuration(uint8_t duration) {
    writeRegister8(MPU6050_REG_FF_DURATION, duration);
}

bool ReefwingMPU6050::getI2CMasterModeEnabled() {
    return readRegisterBit(MPU6050_REG_USER_CTRL, 5);
}

void ReefwingMPU6050::setI2CMasterModeEnabled(bool state) {
    writeRegisterBit(MPU6050_REG_USER_CTRL, 5, state);
}

void ReefwingMPU6050::setI2CBypassEnabled(bool state) {
    return writeRegisterBit(MPU6050_REG_INT_PIN_CFG, 1, state);
}

bool ReefwingMPU6050::getI2CBypassEnabled() {
    return readRegisterBit(MPU6050_REG_INT_PIN_CFG, 1);
}

void ReefwingMPU6050::setAccelPowerOnDelay(OnDelay delay) {
    uint8_t value;

    value = readRegister8(MPU6050_REG_MOT_DETECT_CTRL);
    value &= 0b11001111;
    value |= (delay << 4);
    writeRegister8(MPU6050_REG_MOT_DETECT_CTRL, value);
}

OnDelay ReefwingMPU6050::getAccelPowerOnDelay() {
    uint8_t value;

    value = readRegister8(MPU6050_REG_MOT_DETECT_CTRL);
    value &= 0b00110000;
    return (OnDelay)(value >> 4);
}

uint8_t ReefwingMPU6050::getIntStatus() {
    return readRegister8(MPU6050_REG_INT_STATUS);
}

bool ReefwingMPU6050::dataAvailable() {
    return (getIntStatus() & 0x01);
}

Activites ReefwingMPU6050::readActivites() {
    uint8_t data = readRegister8(MPU6050_REG_INT_STATUS);

    a.isOverflow = ((data >> 4) & 1);
    a.isFreeFall = ((data >> 7) & 1);
    a.isInactivity = ((data >> 5) & 1);
    a.isActivity = ((data >> 6) & 1);
    a.isDataReady = ((data >> 0) & 1);

    data = readRegister8(MPU6050_REG_MOT_DETECT_STATUS);

    a.isNegActivityOnX = ((data >> 7) & 1);
    a.isPosActivityOnX = ((data >> 6) & 1);

    a.isNegActivityOnY = ((data >> 5) & 1);
    a.isPosActivityOnY = ((data >> 4) & 1);

    a.isNegActivityOnZ = ((data >> 3) & 1);
    a.isPosActivityOnZ = ((data >> 2) & 1);

    return a;
}

RawData ReefwingMPU6050::readRawAccel() {
    Wire.beginTransmission(mpuAddress);

    #if ARDUINO >= 100
	    Wire.write(MPU6050_REG_ACCEL_XOUT_H);
    #else
	    Wire.send(MPU6050_REG_ACCEL_XOUT_H);
    #endif

    Wire.endTransmission();

    Wire.beginTransmission(mpuAddress);
    Wire.requestFrom(mpuAddress, 6);

    while (Wire.available() < 6);

    #if ARDUINO >= 100
        uint8_t xha = Wire.read();
        uint8_t xla = Wire.read();
            uint8_t yha = Wire.read();
        uint8_t yla = Wire.read();
        uint8_t zha = Wire.read();
        uint8_t zla = Wire.read();
    #else
        uint8_t xha = Wire.receive();
        uint8_t xla = Wire.receive();
        uint8_t yha = Wire.receive();
        uint8_t yla = Wire.receive();
        uint8_t zha = Wire.receive();
        uint8_t zla = Wire.receive();
    #endif

    ra.rx = (int16_t)(xha << 8 | xla);
    ra.ry = (int16_t)(yha << 8 | yla);
    ra.rz = (int16_t)(zha << 8 | zla);
    ra.timeStamp = micros();

    return ra;
}

ScaledData ReefwingMPU6050::readNormalizeAccel() {
    readRawAccel();

    na.sx = ra.rx * rangePerDigit * 9.80665f;
    na.sy = ra.ry * rangePerDigit * 9.80665f;
    na.sz = ra.rz * rangePerDigit * 9.80665f;
    na.timeStamp = ra.timeStamp;

    return na;
}

ScaledData ReefwingMPU6050::readScaledAccel() {
    readRawAccel();

    na.sx = ra.rx * rangePerDigit;
    na.sy = ra.ry * rangePerDigit;
    na.sz = ra.rz * rangePerDigit;
    na.timeStamp = ra.timeStamp;

    return na;
}


RawData ReefwingMPU6050::readRawGyro() {
    Wire.beginTransmission(mpuAddress);

    #if ARDUINO >= 100
	    Wire.write(MPU6050_REG_GYRO_XOUT_H);
    #else
	    Wire.send(MPU6050_REG_GYRO_XOUT_H);
    #endif

    Wire.endTransmission();

    Wire.beginTransmission(mpuAddress);
    Wire.requestFrom(mpuAddress, 6);

    while (Wire.available() < 6);

    #if ARDUINO >= 100
	    uint8_t xha = Wire.read();
	    uint8_t xla = Wire.read();
        uint8_t yha = Wire.read();
        uint8_t yla = Wire.read();
        uint8_t zha = Wire.read();
        uint8_t zla = Wire.read();
    #else
        uint8_t xha = Wire.receive();
        uint8_t xla = Wire.receive();
        uint8_t yha = Wire.receive();
        uint8_t yla = Wire.receive();
        uint8_t zha = Wire.receive();
        uint8_t zla = Wire.receive();
    #endif

    rg.rx = (int16_t)(xha << 8 | xla);
    rg.ry = (int16_t)(yha << 8 | yla);
    rg.rz = (int16_t)(zha << 8 | zla);
    rg.timeStamp = micros();

    return rg;
}

ScaledData ReefwingMPU6050::readNormalizeGyro() {
    readRawGyro();

    if (useCalibrate) {
        ng.sx = (rg.rx - dg.x) * dpsPerDigit;
        ng.sy = (rg.ry - dg.y) * dpsPerDigit;
        ng.sz = (rg.rz - dg.z) * dpsPerDigit;
    } 
    else {
        ng.sx = rg.rx * dpsPerDigit;
        ng.sy = rg.ry * dpsPerDigit;
        ng.sz = rg.rz * dpsPerDigit;
    }

    if (actualThreshold) {
        if (abs(ng.sx) < tg.x) ng.sx = 0;
        if (abs(ng.sy) < tg.y) ng.sy = 0;
        if (abs(ng.sz) < tg.z) ng.sz = 0;
    }

    ng.timeStamp = rg.timeStamp;

    return ng;
}

TempData ReefwingMPU6050::readTemperature() {
    int16_t T;
    TempData td;

    T = readRegister16(MPU6050_REG_TEMP_OUT_H);
    td.celsius = (float)T/340 + 36.53;
    td.timeStamp = micros();

    return td;
}

int16_t ReefwingMPU6050::getGyroOffsetX() {
    return readRegister16(MPU6050_REG_GYRO_XOFFS_H);
}

int16_t ReefwingMPU6050::getGyroOffsetY() {
    return readRegister16(MPU6050_REG_GYRO_YOFFS_H);
}

int16_t ReefwingMPU6050::getGyroOffsetZ() {
    return readRegister16(MPU6050_REG_GYRO_ZOFFS_H);
}

void ReefwingMPU6050::setGyroOffsetX(int16_t offset) {
    writeRegister16(MPU6050_REG_GYRO_XOFFS_H, offset);
}

void ReefwingMPU6050::setGyroOffsetY(int16_t offset) {
    writeRegister16(MPU6050_REG_GYRO_YOFFS_H, offset);
}

void ReefwingMPU6050::setGyroOffsetZ(int16_t offset) {
    writeRegister16(MPU6050_REG_GYRO_ZOFFS_H, offset);
}

int16_t ReefwingMPU6050::getAccelOffsetX() {
    return readRegister16(MPU6050_REG_ACCEL_XOFFS_H);
}

int16_t ReefwingMPU6050::getAccelOffsetY() {
    return readRegister16(MPU6050_REG_ACCEL_YOFFS_H);
}

int16_t ReefwingMPU6050::getAccelOffsetZ() {
    return readRegister16(MPU6050_REG_ACCEL_ZOFFS_H);
}

void ReefwingMPU6050::setAccelOffsetX(int16_t offset) {
    writeRegister16(MPU6050_REG_ACCEL_XOFFS_H, offset);
}

void ReefwingMPU6050::setAccelOffsetY(int16_t offset) {
    writeRegister16(MPU6050_REG_ACCEL_YOFFS_H, offset);
}

void ReefwingMPU6050::setAccelOffsetZ(int16_t offset) {
    writeRegister16(MPU6050_REG_ACCEL_ZOFFS_H, offset);
}

// Calibrate algorithm
void ReefwingMPU6050::calibrateGyro(uint8_t samples) {
    // Set calibrate
    useCalibrate = true;

    // Reset values
    float sumX = 0;
    float sumY = 0;
    float sumZ = 0;
    float sigmaX = 0;
    float sigmaY = 0;
    float sigmaZ = 0;

    // Read n-samples
    for (uint8_t i = 0; i < samples; ++i) {
        readRawGyro();
        sumX += rg.rx;
        sumY += rg.ry;
        sumZ += rg.rz;

        sigmaX += rg.rx * rg.rx;
        sigmaY += rg.ry * rg.ry;
        sigmaZ += rg.rz * rg.rz;

        delay(5);
    }

    // Calculate delta vectors
    dg.x = sumX / samples;
    dg.y = sumY / samples;
    dg.z = sumZ / samples;

    // Calculate threshold vectors
    th.x = sqrt((sigmaX / samples) - (dg.x * dg.x));
    th.y = sqrt((sigmaY / samples) - (dg.y * dg.y));
    th.z = sqrt((sigmaZ / samples) - (dg.z * dg.z));

    // If already set threshold, recalculate threshold vectors
    if (actualThreshold > 0) {
	    setThreshold(actualThreshold);
    }
}

// Get current threshold value
uint8_t ReefwingMPU6050::getThreshold() {
    return actualThreshold;
}

// Set treshold value
void ReefwingMPU6050::setThreshold(uint8_t multiple) {
    if (multiple > 0) {
        // If not calibrated, need calibrate
        if (!useCalibrate) {
            calibrateGyro();
        }

        // Calculate threshold vectors
        tg.x = th.x * multiple;
        tg.y = th.y * multiple;
        tg.z = th.z * multiple;
    } 
    else {
        // No threshold
        tg.x = 0;
        tg.y = 0;
        tg.z = 0;
    }

    // Remember old threshold value
    actualThreshold = multiple;
}

void ReefwingMPU6050::updateSensorData() {
    if (dataAvailable()) {
        ScaledData gyr = readNormalizeGyro();
        ScaledData acc = readScaledAccel();

        data.gx = gyr.sx;
        data.gy = gyr.sy;
        data.gz = gyr.sz; 
        data.gTimeStamp = gyr.timeStamp;

        data.ax = acc.sx;
        data.ay = acc.sy;
        data.az = acc.sz;
        data.aTimeStamp = acc.timeStamp;

        gyroTemp = readTemperature();
    }
}

InertialMessage ReefwingMPU6050::getInertial() {
    InertialMessage msg;

    if (dataAvailable()) {
        ScaledData gyr = readNormalizeGyro();
        ScaledData acc = readScaledAccel();

        msg.gx = gyr.sx;
        msg.gy = gyr.sy;
        msg.gz = gyr.sz; 
        msg.timeStamp = gyr.timeStamp;

        msg.ax = acc.sx;
        msg.ay = acc.sy;
        msg.az = acc.sz;

        gyroTemp = readTemperature();
    }

    return msg;
}

// Fast read 8-bit from register
uint8_t ReefwingMPU6050::fastRegister8(uint8_t reg) {
    uint8_t value;

    Wire.beginTransmission(mpuAddress);
    #if ARDUINO >= 100
	    Wire.write(reg);
    #else
	    Wire.send(reg);
    #endif

    Wire.endTransmission();

    Wire.beginTransmission(mpuAddress);
    Wire.requestFrom(mpuAddress, 1);

    #if ARDUINO >= 100
	    value = Wire.read();
    #else
	    value = Wire.receive();
    #endif;

    Wire.endTransmission();

    return value;
}

// Read 8-bit from register
uint8_t ReefwingMPU6050::readRegister8(uint8_t reg) {
    uint8_t value;

    Wire.beginTransmission(mpuAddress);

    #if ARDUINO >= 100
	    Wire.write(reg);
    #else
	    Wire.send(reg);
    #endif

    Wire.endTransmission();

    Wire.beginTransmission(mpuAddress);
    Wire.requestFrom(mpuAddress, 1);
    while(!Wire.available()) {};

    #if ARDUINO >= 100
	    value = Wire.read();
    #else
	    value = Wire.receive();
    #endif;

    Wire.endTransmission();

    return value;
}

// Write 8-bit to register
void ReefwingMPU6050::writeRegister8(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(mpuAddress);

    #if ARDUINO >= 100
	    Wire.write(reg);
	    Wire.write(value);
    #else
	    Wire.send(reg);
	    Wire.send(value);
    #endif

    Wire.endTransmission();
}

int16_t ReefwingMPU6050::readRegister16(uint8_t reg) {
    int16_t value;

    Wire.beginTransmission(mpuAddress);

    #if ARDUINO >= 100
        Wire.write(reg);
    #else
        Wire.send(reg);
    #endif

    Wire.endTransmission();

    Wire.beginTransmission(mpuAddress);
    Wire.requestFrom(mpuAddress, 2);
    while(!Wire.available()) {};

    #if ARDUINO >= 100
        uint8_t vha = Wire.read();
        uint8_t vla = Wire.read();
    #else
        uint8_t vha = Wire.receive();
        uint8_t vla = Wire.receive();
    #endif;

    Wire.endTransmission();

    value = vha << 8 | vla;

    return value;
}

void ReefwingMPU6050::writeRegister16(uint8_t reg, int16_t value) {
    Wire.beginTransmission(mpuAddress);

    #if ARDUINO >= 100
        Wire.write(reg);
        Wire.write((uint8_t)(value >> 8));
        Wire.write((uint8_t)value);
    #else
        Wire.send(reg);
        Wire.send((uint8_t)(value >> 8));
        Wire.send((uint8_t)value);
    #endif

    Wire.endTransmission();
}

// Read register bit
bool ReefwingMPU6050::readRegisterBit(uint8_t reg, uint8_t pos) {
    uint8_t value;

    value = readRegister8(reg);
    return ((value >> pos) & 1);
}

// Write register bit
void ReefwingMPU6050::writeRegisterBit(uint8_t reg, uint8_t pos, bool state) {
    uint8_t value;

    value = readRegister8(reg);

    if (state) {
        value |= (1 << pos);
    } 
    else {
        value &= ~(1 << pos);
    }

    writeRegister8(reg, value);
}


