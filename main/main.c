#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "sysenv.h"

#include "beep.h"
#include "spi_mng.h"
#include "button.h"
#include "dac8562.h"
#include "lcd.h"


#include "cur_output.h"
#include "monitor.h"
#include "ads131e0x_hal.h"

#include "dispwin.h"

#include "gatts_server.h"

#include "esp_heap_caps.h"

#define TAG "main"

void heap_caps_alloc_failed_hook(size_t requested_size, uint32_t caps, const char *function_name)
{
    uint32_t ram_left =  heap_caps_get_free_size( MALLOC_CAP_INTERNAL );
    ESP_LOGW(TAG, "%d: - create task free INTERNAL RAM left %dB", __LINE__,ram_left);
    ram_left = heap_caps_get_minimum_free_size(MALLOC_CAP_INTERNAL);
    ESP_LOGW(TAG, "%d: - create task minimum INTERNAL RAM left %dB", __LINE__,ram_left);
    ram_left = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL);
    ESP_LOGW(TAG, "%d: - create task largest INTERNAL RAM left %dB", __LINE__,ram_left);
    printf("%s was called but failed to allocate %d bytes with 0x%X capabilities. \n",function_name, requested_size, caps);
}

void app_main(void)
{   
    int32_t ret;

    //esp_timer_init();
    //load env
    sysenv_load();

    heap_caps_register_failed_alloc_callback(heap_caps_alloc_failed_hook);

    ret = spi_mng_bus_init();
    if(ret < 0)
    {
        ret = -2;
        goto exit;
    }

    BaseType_t rtask = xTaskCreate(monitor, "monitor", 4096, NULL, 5, NULL);
    if(rtask != pdPASS)
    {
        ret = -4;
        goto exit;
    }

    ret = cur_init();
    if(ret < 0)
    {
        ret = -3;
        goto exit;
    }

    ret = lcd_init();
    if(ret < 0)
    {
        ret = -4;
        goto exit;
    }
    
    Key_Init();

    beep_init();
    beep_start(100, 1);

    // vTaskDelay(pdMS_TO_TICKS(3000));
    // lcd_backlight_off();
    // vTaskDelay(pdMS_TO_TICKS(3000));
    // lcd_backlight_on();
    // Key_EventClr();
    // lcd_show_str(120, 45, "Loading...", 0, pic_dc);

    uint32_t cnt = 30;
    uint32_t cc = 0;
    while(cnt--)
    {

        if((0 == Key_GetEvent(KEY_ENTER, KEY_TYPE_STATUS)) && (0 == Key_GetEvent(KEY_DEC, KEY_TYPE_STATUS)))
        {
            cc++;
        }

        if(cc > 10)
        {
            disp_win_debug();
            sysenv_load();
            break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    xTaskCreate(dispwin, "dispwin", 6*1024, NULL, 3, NULL);
    xTaskCreate(ble_task, "ble_task", 8192, NULL, 2, NULL);

    // uint32_t i = 0;
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

exit:
    printf("err %d\n", ret);
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    } 
}


