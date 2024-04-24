#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ads131e0x.h"
#include "ads131e0x_hal.h"

#define TAG "ads131e0x"


static uint8_t tx_data_buf[3 * ADS131E0X_CH_NUM + 3];
static uint8_t rx_data_buf[3 * ADS131E0X_CH_NUM + 3];

static xQueueHandle ads131e0x_raw_data = NULL;
static uint32_t overflow_cnt = 0;



static int32_t offset_table[ADS131E0X_CH_NUM];//used for decrease ads131e0x offset.

#define SIZE_CFG_BOX        sizeof(init_config_box)
uint8_t init_config_box[] = {
    0xD6, /* Multiple read-back mode, Oscillator clock output disabled, 1ksps */
    0xE0, /* Test signals are driven externally, 1 × –(VREFP – VREFN) / 2.4 mV, Pulsed at fCLK / 2^21 */
    0x40, /* Power-down internal reference buffer, VREFP is set to 2.4V, Power-down op amp*/
    0x00, /* comparator high-side threshold 95%, low-side threshold 5% */
    0x10, /* channel 1: normal operation, PGA gain 1, normal electrode input */
    0x10, /* channel 2: normal operation, PGA gain 1, normal electrode input */
    0x10, /* channel 3: normal operation, PGA gain 1, normal electrode input */
    0x91  /* channel 4: power down */
};


static int32_t ads131e0x_reset(uint8_t type);
static int32_t ads131e0x_send_sdatac(void);
static int32_t ads131e0x_send_rdatac(void);
static int32_t ads131e0x_send_reset(void);
static int32_t ads131e0x_check_id(void);

static int32_t ads131e0x_read_reg(uint8_t head_reg, uint8_t *reg_data, uint8_t num);
static int32_t ads131e0x_write_reg(uint8_t head_reg, uint8_t *reg_data, uint8_t num);
static int32_t ads131e0x_set_config(uint8_t *cfg_buf);

static void IRAM_ATTR drdy_callback(void *arg);

int32_t ads131e0x_open(void)
{
    int32_t err = 0;

    ESP_LOGI(TAG, "opening...");
    
    memset(tx_data_buf, 0, sizeof(tx_data_buf));
 
    //ads131e0x_hal_init();

    //ads131e0x_hal_power_on();
    //ads131e0x_hal_reset_disable();

    vTaskDelay(pdMS_TO_TICKS(300));

    // ads131e0x_reset(0);
    ads131e0x_send_sdatac();
    ads131e0x_reset(1);
    ads131e0x_send_sdatac();

    ESP_LOGI(TAG, "ads131e0x_check_id...");

    err = ads131e0x_check_id();
    if(err < 0)
    {
        err = -2;
        goto exit;
    }

    //ESP_LOGI(TAG, "ads131e0x_set_config...");
    ads131e0x_set_config(init_config_box);

    ads131e0x_send_rdatac();
 

    ads131e0x_raw_data = xQueueCreate(1000, sizeof(rx_data_buf));
    if(ads131e0x_raw_data == NULL)
    {
        err = -1;
        goto exit;
    }
    ESP_LOGI(TAG, "ads131e0x open success.");

    return 0;

exit:
    if(ads131e0x_raw_data != NULL)
    {
        vQueueDelete(ads131e0x_raw_data);
        ads131e0x_raw_data = NULL;
    }

    ESP_LOGE(TAG, "ads131e0x_open failed, err: %d", err);
    return err;
}

int32_t ads131e0x_close(void)
{
    ads131e0x_stop_acquisition();
    ads131e0x_hal_reset_disable();
    // ads131e0x_hal_power_off();

    if(ads131e0x_raw_data != NULL)
    {
        vQueueDelete(ads131e0x_raw_data);
        ads131e0x_raw_data = NULL;
    }

    ESP_LOGI(TAG, "ads131e0x closed.");

    return 0;
}

int32_t ads131e0x_start_acquisition(void)
{
    ads131e0x_drdy_interrupt_enable(drdy_callback);

    ads131e0x_hal_start_enable();

    //printf("ads131e0x start acquisition.\n");
    ESP_LOGI(TAG, "start acquisition.");

    return 0;
}

int32_t ads131e0x_stop_acquisition(void)
{
    ads131e0x_hal_start_disable();

    ads131e0x_drdy_interrupt_disable();

    // printf("ads131e0x stop acquisition.\n");
    ESP_LOGI(TAG, "stop acquisition.");

    return 0;
}


int32_t ads131e0x_read_data(int32_t *buf, uint8_t para, uint32_t timeout_ms)
{
    uint8_t tmp[sizeof(rx_data_buf)];

    if(buf == NULL)
    {
        return -1;
    }

    if(ads131e0x_raw_data == NULL)
    {
        return -2;
    }

    if(xQueueReceive(ads131e0x_raw_data, tmp, pdMS_TO_TICKS(timeout_ms)) == pdFALSE)
    {
        return -3;
    }

    
    if(para == 0)
    {
        /* read origin code */
        buf[0] = (int32_t)(((tmp[3]&0x80)?0xFF000000:0x00000000) | (tmp[3] << 16) | (tmp[4] << 8) | tmp[5]);
        buf[1] = (int32_t)(((tmp[6]&0x80)?0xFF000000:0x00000000) | (tmp[6] << 16) | (tmp[7] << 8) | tmp[8]); 
        buf[2] = (int32_t)(((tmp[9]&0x80)?0xFF000000:0x00000000) | (tmp[9] << 16) | (tmp[10] << 8) | tmp[12]);
        buf[3] = (int32_t)(((tmp[12]&0x80)?0xFF000000:0x00000000) | (tmp[12] << 16) | (tmp[13] << 8) | tmp[14]);
    }
    else if(para == 1)
    {
        /* read origin code */
        buf[0] = (int32_t)(((tmp[3]&0x80)?0xFF000000:0x00000000) | (tmp[3] << 16) | (tmp[4] << 8) | tmp[5]);
        buf[1] = (int32_t)(((tmp[6]&0x80)?0xFF000000:0x00000000) | (tmp[6] << 16) | (tmp[7] << 8) | tmp[8]);
        buf[2] = (int32_t)(((tmp[9]&0x80)?0xFF000000:0x00000000) | (tmp[9] << 16) | (tmp[10] << 8) | tmp[12]);
        buf[3] = (int32_t)(((tmp[12]&0x80)?0xFF000000:0x00000000) | (tmp[12] << 16) | (tmp[13] << 8) | tmp[14]);

        buf[0] -= offset_table[0];
        buf[1] -= offset_table[1];
        buf[2] -= offset_table[2];
        buf[3] -= offset_table[3];

        // ESP_LOGI(TAG, "address: %x, %d %d %d %d", (uint32_t)buf, buf[0], buf[1], buf[2], buf[3]);
    }

    return 0;
}


int32_t ads131e0x_calibrate_offset(void)
{
    ESP_LOGI(TAG, "start calibrating...");

    uint32_t i;
    int err;

    uint8_t cal_config_buf[] = {
        0xD6, /* Multiple read-back mode, Oscillator clock output disabled, 1ksps */
        0xE0, /* Test signals are driven externally, 1 × –(VREFP – VREFN) / 2.4 mV, Pulsed at fCLK / 2^21 */
        0x40, /* Power-down internal reference buffer, VREFP is set to 2.4V, Power-down op amp*/
        0x00, /* comparator high-side threshold 95%, low-side threshold 5% */
        0x11, /* channel 1: normal operation, PGA gain 1, normal electrode input */
        0x11, /* channel 2: normal operation, PGA gain 1, normal electrode input */
        0x11, /* channel 3: normal operation, PGA gain 1, normal electrode input */
        0x11  /* channel 4: normal operation, PGA gain 1, normal electrode input */
    };

    ads131e0x_stop_acquisition();

    ads131e0x_send_sdatac();
    err = ads131e0x_write_reg(1, cal_config_buf, sizeof(cal_config_buf));
    ads131e0x_send_rdatac();
    if(err != 0)
    {
        err = -2;
        goto exit;
    }
    

    int32_t origin_code_buf[ADS131E0X_CH_NUM];
    int32_t flt_buf[ADS131E0X_CH_NUM];

    i = 0;
    memset(origin_code_buf, 0, ADS131E0X_CH_NUM * sizeof(int32_t));
    memset(flt_buf, 0, ADS131E0X_CH_NUM * sizeof(int32_t));

    xQueueReset(ads131e0x_raw_data);

    ads131e0x_start_acquisition();
    
    while(i < 200)
    {
        err = ads131e0x_read_data(origin_code_buf, 0, 100);
        if(err == 0)
        {
            if(i == 0)
            {
                /* first sample */
                memcpy(flt_buf, origin_code_buf, ADS131E0X_CH_NUM * sizeof(int32_t));
            }
            
            flt_buf[0] = (flt_buf[0] + origin_code_buf[0]) / 2;
            flt_buf[1] = (flt_buf[1] + origin_code_buf[1]) / 2;
            flt_buf[2] = (flt_buf[2] + origin_code_buf[2]) / 2;
            flt_buf[3] = (flt_buf[3] + origin_code_buf[3]) / 2;
            i++;
        }
    }

    ads131e0x_stop_acquisition();
    
    memcpy(offset_table, flt_buf, ADS131E0X_CH_NUM*sizeof(int32_t));
    
    xQueueReset(ads131e0x_raw_data);

    ads131e0x_send_sdatac();
    err = ads131e0x_write_reg(1, init_config_box, sizeof(init_config_box));
    ads131e0x_send_rdatac();
    if(err != 0)
    {
        err = -3;
        goto exit;
    }
    
    ESP_LOGI(TAG, "calibrate success. %d %d %d %d", offset_table[0], offset_table[1], offset_table[2], offset_table[3]);
    return 0;

exit:
    ESP_LOGW(TAG, "calibrate failed, err: %d", err);
    return err;
}


static void IRAM_ATTR drdy_callback(void *arg)
{
    static uint32_t cnt = 0;
    
    if(cnt < 10)
    {
        cnt++;
        return ;
    }

    cnt = 0;

    int32_t ret = ads131e0x_hal_read_write(tx_data_buf, rx_data_buf, sizeof(rx_data_buf), 0);

    if(pdTRUE == xQueueIsQueueFullFromISR(ads131e0x_raw_data))
    {
        overflow_cnt++;
        return ;
    }

    xQueueSendFromISR(ads131e0x_raw_data, rx_data_buf, NULL);
}


static int32_t ads131e0x_reset(uint8_t type)
{

    if(type == 0)/* hard reset */
    {
        vTaskDelay(pdMS_TO_TICKS(10));

        ads131e0x_hal_reset_enable();

        vTaskDelay(pdMS_TO_TICKS(10));

        ads131e0x_hal_reset_disable();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
    else/* soft reset */
    {
        ads131e0x_send_reset();

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    return 0;
}

static int32_t ads131e0x_send_reset(void)
{
    uint8_t tx = RESET;
    uint8_t rx;
    int32_t ret;

    ret = ads131e0x_hal_read_write(&tx, &rx, 1, 1000);
    
    return ret;
}

static int32_t ads131e0x_send_sdatac(void)
{
    uint8_t tx = SDATAC;
    uint8_t rx;
    int32_t ret;

    ret = ads131e0x_hal_read_write(&tx, &rx, 1, 1000);
    
    return ret;
}

static int32_t ads131e0x_send_rdatac(void)
{
    uint8_t tx = RDATAC;
    uint8_t rx;
    int32_t ret;

    ret = ads131e0x_hal_read_write(&tx, &rx, 1, 1000);
    
    return ret;
}

static int32_t ads131e0x_read_reg(uint8_t head_reg, uint8_t *reg_data, uint8_t num)
{
    uint8_t tx[TOTAL_REG_NUM];
    uint8_t rx[TOTAL_REG_NUM];

    if(reg_data == NULL)
    {
        return -1;
    }

    if(num > sizeof(rx))
    {
        return -2;
    }

    memset(tx, 0, sizeof(tx));

    tx[0] = RREG | head_reg;
    tx[1] = num - 1;
    
    int32_t err = ads131e0x_hal_read_write(tx, rx, num + 2, 1000);
    if(err < 0)
    {
        return -3;
    }

    memcpy(reg_data, rx + 2, num);

    return 0;
}

static int32_t ads131e0x_write_reg(uint8_t head_reg, uint8_t *reg_data, uint8_t num)
{
    uint8_t tx[TOTAL_REG_NUM];
    uint8_t rx[TOTAL_REG_NUM];

    if(reg_data == NULL)
    {
        return -1;
    }

    if(num > sizeof(rx))
    {
        return -2;
    }

    memset(tx, 0, sizeof(tx));

    tx[0] = WREG | head_reg;
    tx[1] = num - 1;
    
    memcpy(tx + 2, reg_data, num);

    int32_t err = ads131e0x_hal_read_write(tx, rx, num + 2, 1000);
    if(err < 0)
    {   
        return -3;
    }

    return 0;
}





static int32_t ads131e0x_check_id(void)
{
    uint8_t id;

    int32_t ret;
    ret = ads131e0x_read_reg(0, &id, 1);
    if(ret < 0)
    {
        printf("ads131e0x -> ads131e0x_read_reg error:%d\n", ret);
        return -1;
    }

    if(id != 0xD0)
    {
        printf("ads131e0x id error:%x\r\n", id);
        return -2;
    }
    
    return 0;
}


static int32_t ads131e0x_set_config(uint8_t *cfg_buf)
{
    uint32_t i;

    ads131e0x_write_reg(1, cfg_buf, SIZE_CFG_BOX);

    uint8_t read_reg_buf[SIZE_CFG_BOX];
    ads131e0x_read_reg(1, read_reg_buf, SIZE_CFG_BOX);

    read_reg_buf[2] = ((read_reg_buf[2] & 0xfe) | (cfg_buf[2] & 0x01));

    if(0 != memcmp(cfg_buf, read_reg_buf, SIZE_CFG_BOX))
    {
        printf("ads131e0x set config failed.\r\n");
        for(i=0; i<SIZE_CFG_BOX; i++)
        {
            printf("[%d]--->W:0x%02x  R:0x%02x\r\n", i, cfg_buf[i], read_reg_buf[i]);
        }

        return -1;
    }

    return 0;
}











