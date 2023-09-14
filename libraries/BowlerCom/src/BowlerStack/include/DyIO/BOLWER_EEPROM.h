/*
 * EEPROM.h
 *
 *  Created on: Feb 17, 2010
 *      Author: hephaestus
 */

#ifndef BOWLER_EEPROM_H_
#define BOWLER_EEPROM_H_



#ifdef __cplusplus
extern "C"{
//#include <../../../../libraries/EEPROM/EEPROM.h>
#endif
#include "BowlerStack/include/Bowler/AbstractPID.h"
#include "BowlerStack/include/DyIO/DyIO_def.h"
#include "BowlerStack/include/Namespace/Namespace_bcs_io.h"

void readPPMLink(uint8_t * vals);
void writePPMLink(uint8_t * vals);

void setEEBrownOutDetect(boolean b);
boolean getEEBrownOutDetect();

void LoadEEstore(void);
void WritePIDvalues(AbsPID * pid, DYIO_PID * dy,int group);
void LoadPIDvals(AbsPID * pid, DYIO_PID * dy,int group);

boolean GetName(char * name);
void SetName(char * name);
boolean GetLockCode(char * name);
void SetLockCode(char * code);
void SetEEPRomData(uint8_t start,uint8_t stop,uint8_t * data);
void GetEEPRomData(uint8_t start,uint8_t stop,uint8_t * data);
void EEInitMAC(void);
boolean ProvisionMAC(uint8_t * m);

void _EEWriteMode(uint8_t pin,uint8_t mode);
void EEWriteValue(uint16_t pin,uint8_t value);
uint8_t EEReadValue(uint16_t pin);
uint8_t EEReadMode(uint8_t pin);

uint32_t EEReadBaud(void);
void EEWriteBaud(uint32_t val);
void EEWriteData(uint16_t addr,uint8_t data);
uint8_t EEReadData(uint16_t addr);


#ifdef __cplusplus
} // extern "C"
#endif
#endif /* EEPROM_H_ */
