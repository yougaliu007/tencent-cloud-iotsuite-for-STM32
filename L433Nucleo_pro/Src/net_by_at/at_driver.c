/*********************************************************************************
 *                   Copyright (c) 2016 - 2020,Tencent
 *                      All rights reserved.
 *
 * File Name:  at_driver.c
 *
 * Description: hardware driver for at cmd send and data recv, device init 
 *
 * History:      <author>          <time>        <version>
 *                   yougaliu          2018-6-7        1.0
 * Desc:           ORG.
 ********************************************************************************/

#include "stm32l4xx_hal.h"
#include "stm32l4xx_it.h"
#include "at_driver.h"
#include "string.h"


extern UART_HandleTypeDef huart1;

static UART_HandleTypeDef *pAtUart = &huart1;
sAtRecvData g_RecvData = {{0,}, 0, eBUFF_RECV_ABLE};




/**
* @brief This function handles USART1 global interrupt.
*/
void USART1_IRQHandler(void)
{
	if(__HAL_UART_GET_IT(&huart1, UART_IT_RXNE) == SET)
	{	
		if(g_RecvData.pos  > (AT_RECIVE_MAX_BUF_LEN - 1))
		{
			g_RecvData.recv_buf[g_RecvData.pos + 1] = '\0';
			g_RecvData.status = eBUFF_FULL;
		}
		else
		{
			g_RecvData.recv_buf[g_RecvData.pos++] = (uint8_t) READ_REG(huart1.Instance->RDR)&0xFF;				
		}

		__HAL_UART_SEND_REQ(&huart1, UART_RXDATA_FLUSH_REQUEST);		
	}
	
}


/**
  *pdata 待发送的数据
  *len 待发送的数据长度
  * @brief At数据发送
  */

HAL_StatusTypeDef at_send_data(uint8_t *pdata, uint16_t len)
{
	return HAL_UART_Transmit(pAtUart, pdata, len, 0xFFFF);
}

/**
  *pCmd 待发送的at 命令数据包
  *atStr 待发送的at命令
  *ackPatten 期待的ack特征串
  *timeOut 超时时间
  * @brief 初始化待发送的At数据
  */
void init_send_cmd(sAtCmd *pCmd,  const char *atStr, const char * ackPatten, uint16_t timeOut)
{
	memset((uint8_t *)pCmd, 0, sizeof(sAtCmd));
	snprintf(pCmd->at_buf, AT_MAX_LEN, "%s", atStr);
	pCmd->timeOut = timeOut;
	
	if(NULL != ackPatten)
	{
		snprintf(pCmd->ack_patten, ACK_MAX_LEN, "%s", ackPatten);
	}

	(uint8_t *)pCmd->pAckBuf = g_RecvData.recv_buf;
		
	return;
}

HAL_StatusTypeDef WaitForAck(uint8_t * pAckpatten,  uint16_t timeOut)
{

	uint32_t ulStartTick;
	uint32_t ulEndTick;
	int Ret;

	if(NULL == pAckpatten)
	{
		return HAL_ERROR;
	}

	ulStartTick = HAL_GetTick();
	ulEndTick = ulStartTick + timeOut;

	 while((ulEndTick > HAL_GetTick())&&(eBUFF_FULL != g_RecvData.status))
	 {
	 	if(NULL != strstr(g_RecvData.recv_buf, pAckpatten))
		{
			Ret = HAL_OK;
			break;
		}
	 }
	 
	if(HAL_ERROR == Ret)
	{
		 DSM_DEBUG("\n\r[WaitForAck]ack_buf[%s] patten[%s] TimeOut ", g_RecvData.recv_buf, pAckpatten);	
	}

	return Ret;
}



/**
  * pCmd:本次要执行的指令，ack内容，及超时时间
  */
HAL_StatusTypeDef exec_gsm_at(sAtCmd *pCmd)
{	
	HAL_StatusTypeDef Ret = HAL_ERROR;
	uint32_t ulStartTick;
	uint32_t ulEndTick;

	if(NULL == pCmd)
	{
		return HAL_ERROR;
	}

	memset((uint8_t *)&g_RecvData, 0, sizeof(sAtRecvData));
	
	ulStartTick = HAL_GetTick();
	ulEndTick = ulStartTick + pCmd->timeOut;

	
	DSM_DEBUG("\n\r[exec_gsm_at] sendcmd: %s", pCmd->at_buf);
	
	
	if(HAL_OK == at_send_data(pCmd->at_buf, strlen(pCmd->at_buf)))
	{
		if(strlen(pCmd->ack_patten) > 0)
		{
			 while((ulEndTick > HAL_GetTick())&&(eBUFF_FULL != g_RecvData.status))
			 {
			 	if(NULL != strstr(g_RecvData.recv_buf, pCmd->ack_patten))
				{
					DSM_DEBUG("\n\rexec Success patten[%s] RecvBuf[%s] ", pCmd->ack_patten, g_RecvData.recv_buf);
					Ret = HAL_OK;

					break;
				}
			 }

			if(HAL_ERROR == Ret)
			{
				 DSM_DEBUG("\n\r[exec_gsm_at] ack_buf[%s] patten[%s] TimeOut ",\
				 	g_RecvData.recv_buf, pCmd->ack_patten);	
			}
		}
		else
		{
			HAL_Delay(pCmd->timeOut);
			Ret = HAL_OK;
		}
		
	}
	else
	{
		DSM_DEBUG("\n\r[exec_gsm_at] at_send_data err");	
	}

	return Ret;
}

/**
  * @brief 关闭回显
  */

HAL_StatusTypeDef atEchoOff(void)
{
	sAtCmd atCmd;	

	//关闭回显	
	init_send_cmd(&atCmd,  "ATE0\r\n", "OK", TIMOUT_3S);
	
	return exec_gsm_at(&atCmd);
}

/**
  * @brief 打开回显
  */

HAL_StatusTypeDef atEchoOn(void)
{
	sAtCmd atCmd;	

	//打开回显	
	init_send_cmd(&atCmd,  "ATE1\r\n", "OK", TIMOUT_1S);
	
	return exec_gsm_at(&atCmd);
}

/**
  * @brief AT 状态确认
  */

HAL_StatusTypeDef atAlive(void)
{
	sAtCmd atCmd;	

	init_send_cmd(&atCmd,  "AT\r\n", "OK", TIMOUT_5S);
	
	return exec_gsm_at(&atCmd);
}



/**
* @ 上电时序要求参见gsm 模组要求
*/
int gsm_poweron(void)
{	

	int Ret = HAL_OK;

	//TO DO .........

	return Ret;	
}



/**
* @ 下电时序要求参见gsm 模组要求
*/
int gsm_powerdown(void)
{
	int Ret = HAL_OK;

	//TO DO .........

	return Ret;
}

/**
* @ gsm 模组硬件初始化
*/
int gsm_hw_init(void)
{
	int Ret = HAL_OK;
	
	Ret = gsm_poweron();

	return Ret;
}


