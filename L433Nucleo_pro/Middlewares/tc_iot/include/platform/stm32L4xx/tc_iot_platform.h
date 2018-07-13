#ifndef TC_IOT_PLATFORM_H
#define TC_IOT_PLATFORM_H

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define tc_iot_hal_malloc malloc
#define tc_iot_hal_free free
#define tc_iot_hal_printf(args...) printf(##args)
#define tc_iot_hal_snprintf snprintf

typedef unsigned long  time_int_type;
struct _tc_iot_timer { time_int_type ts; };

#define IOT_DEBUG(args...)  tc_iot_hal_printf(args)
#define IOT_ERROR(args...)  tc_iot_hal_printf(args)
//#define IOT_INFO(fmt,args...)	tc_iot_hal_printf("%u " fmt , rtc1_counter_get_ms(), args)
#define IOT_INFO(fmt,args...)	tc_iot_hal_printf(fmt,args)


#endif /* end of include guard */
