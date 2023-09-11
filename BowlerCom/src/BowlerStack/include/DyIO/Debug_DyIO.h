/**
 * @fileDebug_DyIO.h
 *
 *  Created on: Aug 25, 2010
 * @author Kevin Harrington
 */

#ifndef DEBUG_DYIO_H_
#define DEBUG_DYIO_H_
#include "BowlerStack/include/DyIO/DyIO_def.h"
#ifdef __cplusplus
extern "C"{
#endif

void printMode(uint8_t pin,uint8_t newMode,  Print_Level l);
#ifdef __cplusplus
} // extern "C"
#endif
#endif /* DEBUG_DYIO_H_ */
