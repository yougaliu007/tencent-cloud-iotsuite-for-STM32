/*********************************************************************************
 *                   Copyright (c) 2016 - 2020,Tencent
 *                      All rights reserved.
 *
 * File Name:    board.h
 *
 * Description:    board drivers for nucleo l433rc-p
 *
 * History:      <author>          <time>        <version>
 *                   yougaliu          2018-6-7        1.0
 * Desc:           ORG.
 ********************************************************************************/

#ifndef __BOARD_H_
#define __BOARD_H_

#define     LOG_NONE     (0x00UL)
#define     LOG_ERROR    (0x01UL)
#define     LOG_WARN     (0x02UL)
#define     LOG_INFO     (0x04UL)
#define     LOG_DEBUG    (0x08UL)
#define     LOG_TRACE    (0x10UL)

#define     G_DEBUG  (LOG_NONE | LOG_ERROR | LOG_WARN | LOG_INFO | LOG_DEBUG )       
#define     LOG_LEVEL_CHECK(level)      (G_DEBUG & level)

#define 	AT_UART_BAUDRATE		(9600)



#endif

