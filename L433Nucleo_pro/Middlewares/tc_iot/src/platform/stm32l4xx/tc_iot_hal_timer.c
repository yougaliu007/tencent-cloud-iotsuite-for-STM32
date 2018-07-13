#ifdef __cplusplus
extern "C" {
#endif

#include "tc_iot_inc.h"
#include "stm32l4xx_hal.h"

//某个移植系统相关的相对时间, 比如是系统启动以来的 ms 数
//#define tc_iot_hal_get_ms   rtc1_counter_get_ms
//#define tc_iot_hal_get_ms	app_timer_cnt_get
unsigned long tc_iot_hal_get_ms()
{
		return HAL_GetTick();
}

void tc_iot_hal_timer_init(tc_iot_timer* timer) {

     timer->ts = tc_iot_hal_get_ms();
}

uint32_t rtc1_get_sys_tick(void)
{
	return HAL_GetTick();
}

char tc_iot_hal_timer_is_expired(tc_iot_timer* timer) {

    time_int_type now_ts = tc_iot_hal_get_ms();

	if (timer->ts <= now_ts)
	{
		return 1;
	}
    return 0;
}

void tc_iot_hal_timer_countdown_ms(tc_iot_timer* timer, unsigned int timeout) {
    timer->ts += timeout;
}

void tc_iot_hal_timer_countdown_second(tc_iot_timer* timer,
                                       unsigned int timeout) {
    tc_iot_hal_timer_countdown_ms(timer, timeout*1000);
}

int tc_iot_hal_timer_left_ms(tc_iot_timer* timer) {
    
    return (int)(timer->ts - tc_iot_hal_get_ms()); 
}

#ifdef __cplusplus
}
#endif
