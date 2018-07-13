#ifdef __cplusplus
extern "C" {
#endif

#include "tc_iot_inc.h"
#include "stm32l4xx_hal.h"
#include "at_driver.h"
#include "at_for_sim900.h"


int tc_iot_get_timestamp(const char* api_url) {
    tc_iot_network_t network;
    tc_iot_http_request request;
    unsigned char http_buffer[1024];
    int max_http_resp_len = sizeof(http_buffer) - 1;
    char temp_buf[TC_IOT_HTTP_MAX_URL_LENGTH];
    int ret;
    char* rsp_body;
	int http_timeout_ms = 2000;
    char http_header[32];
	tc_iot_net_context_init_t netcontext;

    IF_NULL_RETURN(api_url, TC_IOT_NULL_POINTER);


parse_url:

    memset(&network, 0, sizeof(network));

    TC_IOT_LOG_TRACE("request url=%s", api_url);
    if (strncmp(api_url, HTTPS_PREFIX, HTTPS_PREFIX_LEN) == 0) {
#if defined(ENABLE_TLS)
        tc_iot_prepare_network(&network, true, g_tc_iot_https_root_ca_certs);
#else
		 TC_IOT_LOG_TRACE("HTTPS is not support");
#endif
    } else {
		netcontext.use_tls = 0;
        tc_iot_hal_net_init(&network, &netcontext);
        TC_IOT_LOG_TRACE("dirtect tcp network intialized.");
    }

    tc_iot_yabuffer_init(&request.buf, (char *)http_buffer,
                         sizeof(http_buffer));

    TC_IOT_LOG_TRACE("request url=%s", api_url);

    http_header[0] = '\0';

    ret = tc_iot_http_get(&network, &request, api_url,  http_timeout_ms, http_header);
    if (TC_IOT_SUCCESS != ret) {
        TC_IOT_LOG_ERROR("request url=%s failed, ret=%d", api_url, ret);
        return ret;
    }

    ret = network.do_read(&network, (unsigned char *)http_buffer, max_http_resp_len, http_timeout_ms);
    if (ret <= 0) {
        TC_IOT_LOG_ERROR("read from request url=%s failed, ret=%d", api_url, ret);
        return ret;
    }
	else
	{
		TC_IOT_LOG_TRACE("response with:\n%s", http_buffer);   
		rsp_body = strstr(http_buffer, "\r\n\r\n");	
		if(NULL != rsp_body)
		{
			TC_IOT_LOG_TRACE("body:\n%s", rsp_body);   				
		}
		
	}

	network.do_disconnect(&network);
	
    return TC_IOT_SUCCESS;
}


/*
*当前时间到1970-01-01 00:00:00 +0000 (UTC)时间的秒数
*动态token模式下必须实现
*/
long tc_iot_hal_timestamp(void* zone) {	
	return GetTimeStampByGsm(zone);
}

int tc_iot_hal_sleep_ms(long sleep_ms) { 
    HAL_Delay(sleep_ms);
    return 0;
}

 
static unsigned long DJB_hash(const unsigned char* key , int len , unsigned long org_hash)
{
    unsigned long nHash = org_hash;

    while (len-->0)
        nHash = (nHash << 5) + nHash + *key++;

    return nHash;

}

static unsigned int _rand_seed = 0;

//浣跨敤buffer, 鏃堕棿tick鍜屾渶鍒濈殑tick璁＄畻涓�涓殢鏈烘暟
unsigned long tc_iot_random(void* buf,  int len)
{
	unsigned long hash = 0;
	unsigned long tick = rtc1_get_sys_tick(); //涓�涓Щ妞嶅嚱鏁?
	//unsigned long tick = app_timer_cnt_get() * 30;
	unsigned char *strbuf = (unsigned char *) buf;
	_rand_seed ++;

	if (_rand_seed >= len )
		_rand_seed = _rand_seed % len;

	hash = DJB_hash(strbuf + _rand_seed ,  len - _rand_seed , hash);
	if (_rand_seed > 0)
	{
		hash = DJB_hash(strbuf ,  _rand_seed  , hash);
	}

	hash = DJB_hash( (const unsigned char*)&tick, sizeof(tick) , hash  );

	return hash;

}

extern sAtRecvData g_RecvData; 
long tc_iot_hal_random() {
    
    return tc_iot_random(g_RecvData.recv_buf, AT_RECIVE_MAX_BUF_LEN/4);
}

void tc_iot_hal_srandom(unsigned int seed) { 
    _rand_seed = seed ;
}

#ifdef __cplusplus
}
#endif
