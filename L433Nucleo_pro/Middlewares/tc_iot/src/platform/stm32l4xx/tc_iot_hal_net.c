#ifdef __cplusplus
extern "C" {
#endif

#include "tc_iot_inc.h"
#include "tc_iot_hal_timer.h"
#include "stm32l4xx_hal.h"
#include "at_for_sim900.h"
	
/**
 * @brief tc_iot_hal_net_read 接收网络对端发送的数据
 *
 * @param network 网络连接对象
 * @param buffer 接收缓存区
 * @param len 接收缓存区大小
 * @param timeout_ms 最大等待时延，单位ms
 *
 * @return 结果返回码或成功读取字节数, 
 *  假如timeout_ms超时读取了0字节, 返回 TC_IOT_NET_NOTHING_READ
 *  假如timeout_ms超时读取字节数没有达到 len , 返回TC_IOT_NET_READ_TIMEOUT
 *  假如timeout_ms超时对端关闭连接, 返回 实际读取字节数
 * @see tc_iot_sys_code_e
 */
int tc_iot_hal_net_read(tc_iot_network_t* network, unsigned char* buffer,
                        int len, int timeout_ms) {
	tc_iot_timer expired_ts;

    int timeout = timeout_ms;
    
	//IOT_DEBUG("tc_iot_hal_net_read len  %d timeout %d\r\n", len , timeout_ms);
													
    IF_NULL_RETURN(network, TC_IOT_NULL_POINTER);

	int ret = net_read_by_gsm(buffer, len, timeout_ms);


	 if (HAL_ERROR == ret)
    {
        return TC_IOT_NET_NOTHING_READ;
    }
	 
    if (HAL_OK == ret)
    {
        return TC_IOT_SUCCESS;
    }

	if(ret < len)
	{
		return TC_IOT_NET_READ_TIMEOUT;
	}
	
}

int tc_iot_hal_net_write(tc_iot_network_t* network, unsigned char* buffer,
                         int len, int timeout_ms) {
  
    return net_write_by_gsm(buffer, len, timeout_ms);
}




int tc_iot_hal_net_connect(tc_iot_network_t* network, const char* host,
                           uint16_t port) {

    int ret = TC_IOT_SUCCESS;
     
    tc_iot_net_context_t * pNetContext = &(network->net_context);
    int set_ret;
    
    //FUNC_TRACE;
     
    //IOT_DEBUG("tc_iot_hal_net_connect host %s:%d\r\n", host , port);

    if(NULL == network) {
        return TC_IOT_NULL_POINTER;
    }
    
    if (host==NULL)
        host = pNetContext->host;
    if (port==0)
        port = pNetContext->port;
     
    IOT_DEBUG("\r\nnet_connect_by_gsm connect to %s/%d...\r\n", host , port);
     

	if(HAL_OK == net_connect_by_gsm(host, port, pNetContext->extra_context)) 
	{
		IOT_DEBUG("net connect success\n\r");
	}
	else
	{
		IOT_DEBUG("net connect fail\n\r");	
		if(HAL_OK == net_init_by_gsm())  /*重新初始化模组*/
		{
			IOT_DEBUG("net reinit success\n\r");	
			if(HAL_OK == net_connect_by_gsm(host, port, pNetContext->extra_context)) 
			{
				IOT_DEBUG("net connect success\n\r");
				 pNetContext->is_connected = 1;
			}
			else
			{
				IOT_DEBUG("net connect fail\n\r");
				ret = TC_IOT_NET_CONNECT_FAILED;	
			}
		}	
		else
		{
			IOT_DEBUG("net reinit fail\n\r");	
			ret = TC_IOT_NET_CONNECT_FAILED;	
		}
	}	 

     return  ret;
}

int tc_iot_hal_net_is_connected(tc_iot_network_t* network) {

	int ret = TC_IOT_SUCCESS;

	if(HAL_OK == net_connectState_by_gsm(NULL))	
	{
		network->net_context.is_connected = 1;
	}
	else
	{
		network->net_context.is_connected = 0;
		ret = TC_IOT_NET_CONNECT_FAILED;
	}
	
    return ret; 

}

int tc_iot_hal_net_disconnect(tc_iot_network_t* network) {
    tc_iot_net_context_t * pNetContext = &(network->net_context);
    int ret ;

    pNetContext->is_connected = 0;
	
    ret = net_disconnect_by_gsm(pNetContext);
	
    return ret;
}

int tc_iot_hal_net_destroy(tc_iot_network_t* network) {
	int ret = 0;
 
	ret = net_destroy_by_gsm(network);
	
    return ret;
}

int tc_iot_copy_net_context(tc_iot_net_context_t * net_context, tc_iot_net_context_init_t * init) {
    //IF_NULL_RETURN(net_context, TC_IOT_NULL_POINTER);
    //IF_NULL_RETURN(init, TC_IOT_NULL_POINTER);

    net_context->use_tls           = init->use_tls      ;
    net_context->host              = init->host         ;
    net_context->port              = init->port         ;
    net_context->fd                = init->fd           ;
    net_context->is_connected      = init->is_connected ;
    net_context->extra_context     = init->extra_context;

#ifdef ENABLE_TLS
    net_context->tls_config = init->tls_config;
#endif
	  return TC_IOT_SUCCESS;
}

int tc_iot_hal_net_init(tc_iot_network_t* network,
                        tc_iot_net_context_init_t* net_context) {
    if (NULL == network) {
        return TC_IOT_NETWORK_PTR_NULL;
    }

    network->do_read = tc_iot_hal_net_read;
    network->do_write = tc_iot_hal_net_write;
    network->do_connect = tc_iot_hal_net_connect;
    network->do_disconnect = tc_iot_hal_net_disconnect;
    network->is_connected = tc_iot_hal_net_is_connected;
    network->do_destroy = tc_iot_hal_net_destroy;
    tc_iot_copy_net_context(&(network->net_context), net_context);

    return TC_IOT_SUCCESS;
}

#ifdef __cplusplus
}
#endif
