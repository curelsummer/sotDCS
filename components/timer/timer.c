#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include "driver/gpio.h"
#include "esp_log.h"
// #include "soc/timer_group_struct.h"
// #include "driver/periph_ctrl.h"
// #include "driver/timer.h"
#include "timer.h"
#include "button.h"

#define TAG "timer"

static void test_timer_periodic_cb(void *arg);


//定义定时器句柄
esp_timer_handle_t test_p_handle;


//定义一个周期重复运行的定时器结构体
esp_timer_create_args_t test_periodic_arg = { .callback =
		&test_timer_periodic_cb, //设置回调函数
		.arg = NULL, //不携带参数
		.name = "Key_scan_timer" //定时器名字
		};



void test_timer_periodic_cb(void *arg) {

//	int64_t tick = esp_timer_get_time();
	Key_StaUpdate();

}





void timers_init(void) 
{
	//开始创建一个重复周期的定时器并且执行
	// esp_err_t err = esp_timer_create(&test_periodic_arg, &test_p_handle);
	// err = esp_timer_start_periodic(test_p_handle, 50 * 1000);
	// if(err != ESP_OK)
	// {
	// 	ESP_LOGE(TAG, "create timer failed.");
	// }

	TimerHandle_t kst = xTimerCreate("Key_Scan_Timer", pdMS_TO_TICKS(50), pdTRUE, NULL, test_timer_periodic_cb);
	if( kst == NULL )
	{
		ESP_LOGE(TAG, "Creat key scan timer failed.");
	}
	else
	{
		// Start the timer.  No block time is specified, and even if one was
		// it would be ignored because the scheduler has not yet been
		// started.
		if( xTimerStart( kst, 0 ) != pdPASS )
		{
			// The timer could not be set into the Active state.
			ESP_LOGE(TAG, "Start key scan timer failed.");
			xTimerDelete( kst , 0);
		}
	}
}


