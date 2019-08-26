/*
*********************************************************************************************************
*                                              EXAMPLE CODE
*
*                          (c) Copyright 2003-2012; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                      APPLICATION CONFIGURATION
*
*                                     ST Microelectronics STM32
*                                              on the
*
*                                           STM3220G-EVAL
*                                         Evaluation Board
*
* Filename      : app_cfg.h
* Version       : V1.00
* Programmer(s) : DC
*********************************************************************************************************
*/

#ifndef  __APP_CFG_H__
#define  __APP_CFG_H__

#include  <stdio.h>

/*
*********************************************************************************************************
*                                       MODULE ENABLE / DISABLE
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*                                       Low num is the high Prio
*********************************************************************************************************
*/

#define START_TASK_PRIO						  4u   /* Task Prio */

//#define DATA_HANDLE_TASK_PRIO	     		  5u
//#define TCPIP_THREAD_PRIO		              6u
//#define LWIP_DHCP_TASK_PRIO                 7u

/* Bigger is lower */
#define SERL_COMM_TASK_PRIO                  (5u)

#define BASE_CFG_TASK_PRIO                   (6u)

#define INTER_COMM_TASK_PRIO                 (7u)


#define TCP_CLIENT_PRIO		                  10u

#define TCP_CLIENT_CONN_PRIO				  11u

#define TCP_CLIENT_SEND_PRIO				  12u

#define TCP_CLIENT_RECV_PRIO                  13u

#define UDP_CLIENT_CONN_PRIO				  14u

#define UDP_CLIENT_SEND_PRIO				  15u

#define UDP_CLIENT_RECV_PRIO                  16u

#define SOCKET_UDP_PRIO                       17u

#define LED_TASK_PRIO						  (OS_LOWEST_PRIO-6u)
#define TASK_CHECK_PRIO                       (OS_LOWEST_PRIO-5u)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define START_STK_SIZE		                 128u  /* Task Stack Size */
#define LWIP_DHCP_STK_SIZE  		         128u
#define DATA_HANDLE_STK_SIZE				 512u
#define LED_STK_SIZE						 128u
#define TASK_CHECK_STK_SIZE				     128u
#define TCP_CLIENT_STK_SIZE	                 512u
#define TCP_CLIENT_CONN_STK_SIZE			 128u
#define TCP_CLIENT_SEND_STK_SIZE		     1128u
#define TCP_CLIENT_RECV_STK_SIZE             128u
#define UDP_CLIENT_CONN_STK_SIZE			 128u
#define UDP_CLIENT_SEND_STK_SIZE		     128u
#define UDP_CLIENT_RECV_STK_SIZE             128u
#define SOCKET_UDP_STK_SIZE                  512u

/* Added by wangdong begin[2018.04.03]*/
#define INTER_COMM_TASK_STK_SIZE             (512u)  /* 2 KB */
#define SERL_COMM_TASK_STK_SIZE              (512u)  /* 2 KB */
#define BASE_CFG_TASK_STK_SIZE               (512u)  /* 2 KB */
/* Added by wangdong end */

/*
*********************************************************************************************************
*                                            BSP CFG
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/


#endif

