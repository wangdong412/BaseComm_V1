#include "mv88e6061_phy.h"


uint32 MV88E6061IDGet(uint32 phyAddr)
{
    uint32 id1 = 0U, id2 = 0, id = 0;

    /* read the ID1 register */
	id1 = ETH_ReadPHYRegister(phyAddr, MV88E6061_PHY_ID1);

    /* update the ID1 value */
    id = (uint32)((uint32)id1 << MV88E6061_PHY_ID_SHIFT);

    /* read the ID2 register */
	id2 = ETH_ReadPHYRegister(phyAddr, MV88E6061_PHY_ID2);

    /* update the ID2 value */
    id |= id2;

    /* return the ID in ID1:ID2 format */
    return id;
}

void MV88E6061_SoftReset(uint32 phyAddr)
{
    uint16 regVal = 0U;
	
	ETH_WritePHYRegister(phyAddr, MV88E6061_PHY_BCR, MV88E6061_PHY_SOFTRESET);
	regVal = ETH_ReadPHYRegister(phyAddr, MV88E6061_PHY_BCR);
	
    while((regVal & MV88E6061_PHY_SOFTRESET) != 0U)
    {
        regVal = ETH_ReadPHYRegister(phyAddr, MV88E6061_PHY_BCR);
    }	
}

u16 MV88E6061_ReadReg(uint32 phyAddr, uint16 phyreg)
{
	u16 ret = 0;
	
	ret = ETH_ReadPHYRegister(phyAddr, phyreg);
	return ret;
}


void MV88E6061_WriteReg(uint32 phyAddr, uint16 phyreg, uint16 value)
{
	ETH_WritePHYRegister(phyAddr, phyreg, value);
}




















