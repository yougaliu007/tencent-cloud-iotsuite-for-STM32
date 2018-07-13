#ifndef __GSM_DRV_H_
#define __GSM_DRV_H_

#define AT_MAX_LEN		(128)
#define ACK_MAX_LEN		(128)
#define AT_RECIVE_MAX_BUF_LEN	(2048)

#define TIMOUT_1S		(1000)
#define TIMOUT_2S		(2000)
#define TIMOUT_3S		(3000)
#define TIMOUT_4S		(4000)
#define TIMOUT_5S		(5000)


#define IP_CMD_LEN				(128)
#define READ_CMD_ACK_PATTEN		"+CIPRXGET: 2,"

//#define GSM_DEBUG


#ifdef GSM_DEBUG
//#define DSM_DEBUG(fmt, args...)  printf(fmt, args...)

#define DSM_DEBUG(args...)\
	do {\
	   printf(##args); \
   }while(0)

#else
#define DSM_DEBUG(fmt, args...) HAL_Delay(10)
#endif

#define END_PATTEN1			(0X0A)
#define END_PATTEN2			(0X0D)



typedef enum at_recv_status
{	
	eBUFF_RECV_ABLE = 0,
	eBUFF_READY_FOR_DEAL,	
	eBUFF_FULL,
}eBufStatus;

typedef struct at_cmd_struct
{
    uint8_t  at_buf[AT_MAX_LEN];   
    uint8_t  ack_patten[ACK_MAX_LEN];
	uint16_t timeOut;
	void *	 pAckBuf;
}sAtCmd;


typedef struct at_recv_buf_struct
{
    uint8_t  	 recv_buf[AT_RECIVE_MAX_BUF_LEN + 1];     
	uint16_t	 pos;
	eBufStatus 	 status;
}sAtRecvData;



int gsm_hw_init(void);
void init_send_cmd(sAtCmd *pCmd,  const char *atStr, const char * ackPatten, uint16_t timeOut);
HAL_StatusTypeDef at_send_data(uint8_t *pdata, uint16_t len);
HAL_StatusTypeDef exec_gsm_at(sAtCmd *pCmd);
HAL_StatusTypeDef WaitForAck(uint8_t * pAckpatten,  uint16_t timeOut);
HAL_StatusTypeDef atEchoOff(void);
HAL_StatusTypeDef atEchoOn(void);
HAL_StatusTypeDef atAlive(void);


#endif

