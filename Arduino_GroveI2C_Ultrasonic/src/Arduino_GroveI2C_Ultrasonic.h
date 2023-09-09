/*
  This file is part of the Arduino_GroveI2C_Ultrasonic library.
  Copyright (c) 2023 Arduino SA. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __ARDUINO_GROVEI2C_ULTRASONIC_H__
#define __ARDUINO_GROVEI2C_ULTRASONIC_H__

#include "Arduino.h"
#include "Wire.h"

#define ULTRASONIC_ADDRESS 0x57

class Arduino_GroveI2C_Ultrasonic {
  private:
    uint8_t address;
    TwoWire& wire;
    float measure;
    uint32_t data;

  public:
    Arduino_GroveI2C_Ultrasonic(TwoWire & _wire = Wire, uint8_t _address = ULTRASONIC_ADDRESS);
    void begin();
    void update();              // update internal value
    float getDistance();        // mm
    float getMeters();          // m
    float getTravelTime();      // us
    bool checkConnection();     // check if it is connected
};

#endif