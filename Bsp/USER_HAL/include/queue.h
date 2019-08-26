
/***************************************************************************
 *   Copyright(C)2009-2012 by Gorgon Meducer<Embedded_zhuoran@hotmail.com> *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef __QUEUE_H_
#define __QUEUE_H_

#include "stdint.h"
#include "stdio.h"
#include "stm32f4xx.h"

#ifndef  FALSE
#define  FALSE                       0u
#endif

#ifndef  TRUE
#define  TRUE                        1u
#endif

typedef uint8_t BOOL;

/*============================ TYPES =========================================*/




typedef struct 
{
#ifdef SOCKET_SN_ENABLE	
	u32 sn;
#endif
	
#ifdef QUEUE_TIME_EN
    u32 timeS;
	u32 timeE;
	u32 err;
	u32 idle;
#endif
    u8 data[128];
	u8 index;
	u8 len;	
}Data_t;
#define DATA_HEAD_LEN		(10)

typedef struct 
{
    Data_t            *ptBuffer;
    unsigned int      tSize;                                                  
    unsigned int      tHead;                                                  
    unsigned int      tTail;                                                  
    unsigned int      tCounter;                                               
}Queue_t;


/*============================ GLOBAL VARIABLES ==============================*/
#define QUEUE_SIZE	         50
extern Data_t Queue_Data1Buf[QUEUE_SIZE]; 
extern u8 Queue_Index;


/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/
extern BOOL Queue_Init(void);
extern BOOL Queue_Out(Data_t *data);
extern BOOL Queue_In(Data_t *data);

#endif
