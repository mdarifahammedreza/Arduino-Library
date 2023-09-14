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

#include "Arduino_GroveI2C_Ultrasonic.h"


Arduino_GroveI2C_Ultrasonic::Arduino_GroveI2C_Ultrasonic(TwoWire & _wire, uint8_t _address):wire(_wire){
  address=_address;
}

void Arduino_GroveI2C_Ultrasonic::begin(){
  wire.begin();
  update();
}

void Arduino_GroveI2C_Ultrasonic::update(){
  wire.beginTransmission(address);
  wire.write(0x01);
  wire.endTransmission();
  delay(5);
  wire.requestFrom(address,(uint8_t)3);
  data = wire.read();
  data <<= 8;
  data |= wire.read();
  data <<= 8;
  data |= wire.read();
  measure = float(data) / 1000;
  if (measure > 4500.00) {
    if (measure == 4294967.50){
      measure = -1.0;
    }
    else{
      measure = 4500.00;
    }
  }
}

float Arduino_GroveI2C_Ultrasonic::getDistance(){
  return measure;
}

float Arduino_GroveI2C_Ultrasonic::getMeters(){
  return measure/1000.0;
}

bool Arduino_GroveI2C_Ultrasonic::checkConnection(){
  update();
  if (measure==-1.0){
    return false;
  }
  return true;
}

float Arduino_GroveI2C_Ultrasonic::getTravelTime(){
  return measure*2.0/0.343; // 2 times the distance at the speed of sound in mm/us
}