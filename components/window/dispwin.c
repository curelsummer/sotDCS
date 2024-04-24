#include <stdio.h>
#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "sysenv.h"

#include "win_basic.h"
#include "dispwin.h"

#define TAG "dispwin"


static void set_next_win(int32_t ret);

static int32_t pattern;

void dispwin(void *arg)
{
    pattern = sysenv_get_device_mode();
    pattern = WIN_ID_TACS;

    while(1)
    {
        switch(pattern)
        {
            case WIN_ID_TDCS:
                ESP_LOGI(TAG, "enter tdcs");
                set_next_win(disp_win_tdcs());

                break;

            case WIN_ID_TACS:
                ESP_LOGI(TAG, "enter tacs");
                set_next_win(disp_win_tacs());

                break;

            case WIN_ID_CES:
                ESP_LOGI(TAG, "enter ces");
                set_next_win(disp_win_ces());
                
                break;
            
            case WIN_ID_RTACS:
                ESP_LOGI(TAG, "enter rtacs");
                set_next_win(disp_win_rtacs());
                
                break;

            case WIN_ID_HOME_TDCS:
                ESP_LOGI(TAG, "enter home tdcs");
                set_next_win(disp_win_home_tdcs());
                
                break;

            case WIN_ID_HOME_CES:
                ESP_LOGI(TAG, "enter home ces");
                set_next_win(disp_win_home_ces());
                
                break;

            default:
                ESP_LOGW(TAG, "ERROR WIN ID: %d", pattern);
                set_next_win(WIN_ID_TDCS);
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void set_next_win(int32_t next_win)
{
    switch(next_win)
    {
        case WIN_ID_TDCS:
        case WIN_ID_TACS:
        case WIN_ID_CES:
            pattern = next_win;
            break;
            
        case WIN_CUR: 
            pattern = pattern;
            break;

        case WIN_NEXT: 
            if(pattern == WIN_ID_CES)
            {
                pattern = WIN_ID_TDCS;
            }
            else
            {
                pattern += 1;
            }
            break;

        case WIN_PRE:
            if(pattern == WIN_ID_TDCS)
            {
                pattern = WIN_ID_CES;
            }
            else
            {
                pattern -= 1;
            }
            break;

    }
}
