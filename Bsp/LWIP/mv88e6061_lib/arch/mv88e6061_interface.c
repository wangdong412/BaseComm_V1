#include "msApi.h"
#include "mv88e6061_interface.h"
#include "includes.h"

GT_SYS_CONFIG   cfg;
GT_QD_DEV       diagDev;
GT_QD_DEV       *dev=&diagDev;

GT_BOOL MV88E6061ReadMii(GT_QD_DEV* dev, unsigned int portNumber, unsigned int MIIReg, unsigned int* value)
{
	u16 ret = 0;
	
	*value = ETH_ReadPHYRegister(portNumber, MIIReg);

	return TRUE;
}

GT_BOOL MV88E6061WriteMii(GT_QD_DEV* dev, unsigned int portNumber, unsigned int MIIReg, unsigned int value)
{
	u32 ret = 0;
	
	ret = ETH_WritePHYRegister(portNumber, MIIReg, value);
	if(ret == ETH_SUCCESS)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void MV88E6061_ModeInit(void)
{
    GT_STATUS status;
    GT_BOOL tmpmode = GT_FALSE;
    GT_LPORT tmpport;
    /*
     * zero the cfg/diagDev.
     */

    /*
     * register all the required functions to QuarterDeck Driver.
     */
    cfg.BSPFunctions.readMii = MV88E6061ReadMii;
    cfg.BSPFunctions.writeMii = MV88E6061WriteMii;
    /* see Software Arch Spec (MV-S800039-00I-88E6061.pdf) chapter 2.3.2.1
     * only one task is accessing any one of the above features at a given time,
     * none of the optional semaphore functions are required.
     */
    cfg.BSPFunctions.semCreate = 0;
    cfg.BSPFunctions.semDelete = 0;
    cfg.BSPFunctions.semTake = 0;
    cfg.BSPFunctions.semGive = 0;

    cfg.initPorts = GT_TRUE; /* Set switch ports to Forwarding mode. If GT_FALSE, use Default Setting. */
    cfg.cpuPortNum = 5;

    cfg.mode.scanMode = SMI_AUTO_SCAN_MODE; /* Scan 0 or 0x10 base address to find the QD */
    cfg.mode.baseAddr = 0;	
	
    if(GT_OK != qdLoadDriver(&cfg, dev))
    {
		printf("MV Switch: qdLoadDriver failed\r\n");
    }	
	
    printf("Device ID     : 0x%x\n",dev->deviceId);
    printf("Base Reg Addr : 0x%x\n",dev->baseRegAddr);
    printf("No of Ports   : %d\n",  dev->numOfPorts);
    printf("CPU Ports     : %d\n",  dev->cpuPortNum);	
	
    /*
     * start the QuarterDeck
     */
    if((status=sysEnable(dev)) != GT_OK)
    {
        printf("MV Switch: sysEnable failed\n");                  
    }

    /* zk: MV88E6061: WITH CTRL MODE 0/1, STATISTICS COUNTERS HAVE DIFFERENT MEANINGS  */
    if(GT_OK != gprtSetCtrMode(dev, 1))
    {
		printf("gprtSetCtrMode  failed\n");
    }
	
	/* zk: temporary function: forward frames to all ports on MV88E6061 for debug purposes*/
	  for(tmpport = 0; tmpport < dev->numOfPorts; tmpport++)
	{
		 gprtSetMapDA(dev, tmpport, 0);
		 gprtGetMapDA(dev, tmpport, &tmpmode);
	   printf(" port %d EgressFlood     : 0x%x\n", tmpport, tmpmode);
	}
	  /*
88E6061 VLAN Table configuration
1. All used ports are connected to CPU (P2) only.
2. The unused ports, like P4, is disabled.

	      P5  | P4   | P3   | P2  | P1 | P0
	      MCU | null | Wifi | CPU | E1 | E0
	     -----+------+------+-----+----+---
P0 E0	   0     0      0      1    0    0    0x04
P1 E1	   0     0      0      1    0    0    0x04
P2 CPU	   1     0      1      0    1    1    0x2B
P3 Wifi    0     0      0      1    0    0    0x04
P4 null    0     0      0      0    0    0    0x00
P5 MCU     0     0      0      1    0    0    0x04
*/
	ETH_WritePHYRegister(0x18, 6, 0x04);
	ETH_WritePHYRegister(0x19, 6, 0x04);
	ETH_WritePHYRegister(0x1A, 6, 0x2B);
	ETH_WritePHYRegister(0x1B, 6, 0x04);
	ETH_WritePHYRegister(0x1C, 6, 0x00);
	ETH_WritePHYRegister(0x1D, 6, 0x04);
}

























