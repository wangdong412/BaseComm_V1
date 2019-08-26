#ifndef __MV88E6061_H__
#define __MV88E6061_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"


#define MV88E6061_PHY_ADDRESS  (0x03)

extern u8 MV88E6061_Init(void);
extern void MV88E6061_GetSpeed(void);
extern void MV88E6061_Test(void);


#endif

