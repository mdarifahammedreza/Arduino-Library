/*
 * PID.h
 *
 *  Created on: Feb 6, 2010
 *      Author: hephaestus
 */

#ifndef PID_H_
#define PID_H_
#ifdef __cplusplus
extern "C"{
#endif


void GetConfigDyPID(BowlerPacket * Packet);
uint8_t ConfigDyPID(BowlerPacket * Packet);
void runPIDConfigurationValueSync();
void InitPID(AbsPID * pidGroups,DYIO_PID * dyPid,PidLimitEvent * limits);

void trigerPIDLimit(uint8_t chan,PidLimitType type,int32_t  tick);

uint8_t GetPIDGroup(uint8_t channel);
void pushPPMPacket(void);
#ifdef __cplusplus
} // extern "C"
#endif
#endif /* PID_H_ */
