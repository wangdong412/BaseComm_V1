#ifndef __MV88E6061_PHY_H__
#define __MV88E6061_PHY_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* PHY register offset definitions */
#define MV88E6061_PHY_BCR                           (0u)
#define MV88E6061_PHY_BSR                           (1u)
#define MV88E6061_PHY_ID1                           (2u)
#define MV88E6061_PHY_ID2                           (3u)
#define MV88E6061_PHY_AUTONEG_ADV                   (4u)
#define MV88E6061_PHY_LINK_PARTNER_ABLTY            (5u)

#define MV88E6061_PHY_ID_SHIFT                      (16u)
#define MV88E6061_PHY_SOFTRESET                     (0x8000U)

extern uint32 MV88E6061IDGet(uint32 phyAddr);
extern void MV88E6061_SoftReset(uint32 phyAddr);
extern u16 MV88E6061_ReadReg(uint32 phyAddr, uint16 phyreg);
extern void MV88E6061_WriteReg(uint32 phyAddr, uint16 phyreg, uint16 value);

#endif



