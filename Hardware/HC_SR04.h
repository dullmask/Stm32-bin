#ifndef __HC_SR04_H
#define __HC_SR04_H	 
#include "sys.h"

#define SR04_Trlg PAout(3)
#define SR04_Echo PAin(4)

void HC_SR04_IO_Init(void);
u16  Get_SR04_Distance(void);

#endif
