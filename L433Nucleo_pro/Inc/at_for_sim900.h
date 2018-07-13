/*********************************************************************************
 *                   Copyright (c) 2016 - 2020,Tencent
 *                      All rights reserved.
 *
 * File Name:   at_for_sim900.h
 *
 * Description:   net api based on at cmd for sim900
 *
 * History:      <author>          <time>        <version>
 *                   yougaliu          2018-6-7        1.0
 * Desc:           ORG.
 ********************************************************************************/

#ifndef __NET_GSM_H_
#define __NET_GSM_H_

#define NET_SEND_MAXLEN_ONCE		(1352)		//sim900??η?????????
#define NET_READ_MAXLEN_ONCE		(1460)      //sim900 ????????????????ζ???????????

#define RET_OK						(0)
#define RET_ERR						(-1)

#define TIME_STR_LEN				(32)
#define TIME_BASE_YEAR				(2000)
#define TIME_OFFSET					(167952976)


int net_init_by_gsm(void);
int net_connect_by_gsm(const char *host,  int port,  void* pdata);
int net_disconnect_by_gsm(void* pdata);
int net_connectState_by_gsm(void* pdata);
int net_destroy_by_gsm(void* pdata);
int net_read_by_gsm(uint8_t *pRecvbuf, int len, uint16_t timeOut);
int net_write_by_gsm(uint8_t *pSendbuf, int len, uint16_t timeOut);
int GetLocalTimeByGsm(uint8_t * strTime);
long GetTimeStampByGsm(void *zone);

#endif

