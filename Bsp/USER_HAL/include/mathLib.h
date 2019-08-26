#ifndef __BSP_MATH_LIB_H__
#define __BSP_MATH_LIB_H__

#include "includes.h"

extern u16 xMathCRC16(u8 pucFrame[], u16 usLen);
extern u16 xBEBufToUint16(u8 _pBuf[]);
extern u16 xLEBufToUint16(u8 _pBuf[]);
extern void vBEUint16ToBuf(u16 data, u8 _pBuf[]);
extern void vLEUint16ToBuf(u16 data, u8 _pBuf[]);
extern u16 xMathCheckSum(u8 _pBuf[], u16 _usLen);
extern void vMathMemset(u8 str[], u8 c, u32 count);
extern void vMathMemcpy(u8 dst[], u8 src[], u32 count);

#endif

