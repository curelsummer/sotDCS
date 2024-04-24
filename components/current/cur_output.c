#include <stdint.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"

#include "nvs_flash.h"
#include "nvs.h"

#include "esp_err.h"
#include "esp_log.h"

#include "cur_output.h"
#include "dac8562.h"

#define TAG "cur_output"


#define NVS_NAMESPACE      "std"
#define NVS_CALI_KEY       "cali_cur"

#define CUR_OUTPUT_EN_NUM              (21)
#define CUR_POWER_EN_NUM               (19)
#define CUR_RELAY_EN_NUM                (9)

#define UA_TO_CODE(n) ((n) * 6.5536)


static SemaphoreHandle_t cur_mutex = NULL;


static float cali_a0 = 0;
static float cali_a1 = 1;

static int32_t load_cali_cur_para(cali_cur_para_t *para);

static int32_t cur_mod_lock(uint32_t timeout_ms);
static int32_t cur_mod_unlock(void);
static int32_t cur_ctlio_init(void);


int32_t cur_init(void)
{
    int32_t ret = 0;

    if(cur_mutex == NULL)
    {
        nvs_flash_init();

        cur_ctlio_init();
    
        dac8562_init();

        cali_cur_para_t para;
        ret = load_cali_cur_para(&para);
        if(ret < 0)
        {
            cali_a0 = 0;
            cali_a1 = 1;

            ESP_LOGI(TAG, "can not find cali para: a0 = %f, a1 = %f", cali_a0, cali_a1);
        }
        else
        {
            cali_a0 = para.a0;
            cali_a1 = para.a1;

            ESP_LOGI(TAG, "load cali para from nvs: a0 = %f, a1 = %f", cali_a0, cali_a1);
        }

        cur_set_ua(0, 1);


        cur_mutex = xSemaphoreCreateMutex();
        if(cur_mutex == NULL)
        {
            ret = -1;
            goto exit;
        }

        return 0;
    }
    else
    {
        return 1;
    }

exit:
    if(cur_mutex != NULL)
    {
        vSemaphoreDelete(cur_mutex);
        cur_mutex = NULL;
    }

    return ret;
}

int32_t cur_open(uint32_t timeout_ms)
{
    int32_t ret;
    ret = cur_mod_lock(timeout_ms);
    if(ret < 0)
    {
        ret = -1;
        goto exit;
    }

    //read calibration info from nvs
    cali_cur_para_t para;
    ret = load_cali_cur_para(&para);
    if(ret < 0)
    {
        cali_a0 = 0;
        cali_a1 = 1;

        ESP_LOGI(TAG, "can not find cali para: a0 = %f, a1 = %f", cali_a0, cali_a1);
    }
    else
    {
        cali_a0 = para.a0;
        cali_a1 = para.a1;

        ESP_LOGI(TAG, "load cali para from nvs: a0 = %f, a1 = %f", cali_a0, cali_a1);
    }

    cur_set_ua(0, 1);

    cur_power_on();

    // vTaskDelay(pdMS_TO_TICKS(1000));

    // cur_enable_output();
    
    return 0;

exit:
    cur_mod_unlock();
    return ret;
}

int32_t cur_close(void)
{
    cur_disable_output();
    cur_power_off();
    cur_set_ua(0, 1);

    int32_t ret;
    ret = cur_mod_unlock();

    return ret;
}

int32_t cur_power_on(void)
{
    gpio_set_level(CUR_POWER_EN_NUM, 1);

    return 0;
}

int32_t cur_power_off(void)
{
    gpio_set_level(CUR_POWER_EN_NUM, 0);
    
    return 0;
}

int32_t cur_enable_output_relay(void)
{
    gpio_set_level(CUR_RELAY_EN_NUM, 1);

    return 0;
}


int32_t cur_disable_output_relay(void)
{
    gpio_set_level(CUR_RELAY_EN_NUM, 0);

    return 0;
}


int32_t cur_enable_output(void)
{
    gpio_set_level(CUR_OUTPUT_EN_NUM, 1);

    cur_enable_output_relay();
    return 0;
}


int32_t cur_disable_output(void)
{
    gpio_set_level(CUR_OUTPUT_EN_NUM, 0);

    cur_disable_output_relay();
    return 0;
}


int32_t cur_convert_ua_to_code(float ua)
{
    return UA_TO_CODE(cali_a0 + ua * cali_a1) + 0.5;
}

int32_t cur_set_code(int32_t code)
{
    //printf("%d\n", code);
    dac8562_output_b(code + 0x7FFF);
    return 0;
}

//can not run in interrupt
int32_t cur_set_ua(float ua, uint8_t cali_flag)
{
    int32_t code;
    // ESP_LOGI(TAG, "SET ua: %f", ua);

    if(0 == cali_flag)
    {
        code = UA_TO_CODE(ua);
    }
    else
    {
        code = UA_TO_CODE(cali_a0 + ua * cali_a1);
    }

    cur_set_code(code);

    return 0;
}

cur_port_t cur_get_port(uint32_t port)
{
    switch(port)
    {
        case 0:
            return CUR_PORT_0;

        default:
            return CUR_PORT_ERR;
    }
}


static int32_t cur_ctlio_init(void)
{
    gpio_config_t io_conf;

    cur_disable_output_relay();
    io_conf.pin_bit_mask = 1ULL<<CUR_RELAY_EN_NUM;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en =0;
    gpio_config(&io_conf);

    cur_power_off();
    io_conf.pin_bit_mask = 1ULL<<CUR_POWER_EN_NUM;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en =0;
    gpio_config(&io_conf);

    cur_disable_output();
    io_conf.pin_bit_mask = 1ULL<<CUR_OUTPUT_EN_NUM;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en =0;
    gpio_config(&io_conf);

    return 0;
}

static int32_t cur_mod_lock(uint32_t timeout_ms)
{
    if(pdTRUE != xSemaphoreTake( cur_mutex, pdMS_TO_TICKS(timeout_ms)))
    {
        return -1;
    }

    return 0;
}

static int32_t cur_mod_unlock(void)
{
    if(pdTRUE != xSemaphoreGive( cur_mutex ))
    {
        return -1;
    }

    return 0;
}

static int32_t load_cali_cur_para(cali_cur_para_t *para)
{
    if(para == NULL)
    {
        return -1;
    }

    int32_t ret = 0;
    esp_err_t err;
    nvs_handle nvs_handle;

    // err = nvs_flash_init();
    // if(ret != ESP_OK)
    // {
    //     ESP_LOGE(TAG, "NVS init failed.");
    //     return -2;
    // }

    err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &nvs_handle);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs_read_device_id: open nvs failed.(%d)", err);
        ret = -3;
        goto exit;
    }

    size_t length;
    err = nvs_get_blob(nvs_handle, NVS_CALI_KEY, para, &length);
    if(err != ESP_OK)
    {
        ESP_LOGE(TAG, "nvs_write_device_id: get calibration failed.(%d)", err);
        ret = -4;
        goto exit;
    }

exit:
    nvs_close(nvs_handle);
    return ret;
}