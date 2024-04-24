#include <stdint.h>
#include <string.h>

#include "esp_err.h"

#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "spi_mng.h"

#include "ads131e0x_hal.h"



static spi_device_handle_t spi_ads131e0x;


static void init_function_io(void);
static int32_t spi_interface_init(void);

int32_t ads131e0x_hal_init(void)
{
    esp_err_t err;

    init_function_io();

    err = gpio_install_isr_service(0);
    if(err != ESP_OK)
    {
        return -1;
    }

    err = spi_interface_init();
    if(err != ESP_OK)
    {
        return -2;
    }

    return 0;
}


//  int32_t ads131e0x_hal_power_on(void)
// {
//     gpio_set_level(ADS131E0X_PWDN_NUM, 1);
//     return 0;
// }

// int32_t ads131e0x_hal_power_off(void)
// {
//     gpio_set_level(ADS131E0X_PWDN_NUM, 0);
//     return 0;
// }

int32_t ads131e0x_hal_reset_enable(void)
{
    gpio_set_level(ADS131E0X_RST_NUM, 0);
    return 0;
}

int32_t ads131e0x_hal_reset_disable(void)
{
    gpio_set_level(ADS131E0X_RST_NUM, 1);
    return 0;
}


int32_t ads131e0x_hal_start_enable(void)
{
    gpio_set_level(ADS131E0X_START_NUM, 1);
    return 0;
}

int32_t ads131e0x_hal_start_disable(void)
{
    gpio_set_level(ADS131E0X_START_NUM, 0);
    return 0;
}

static void init_function_io(void)
{
    gpio_config_t io_conf;

    // ads131e0x_hal_power_off();
    // io_conf.pin_bit_mask = ADS131E0X_PWDN_SEL;//pwdn pin
    // io_conf.mode = GPIO_MODE_OUTPUT;
    // io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    // io_conf.pull_down_en = 0;
    // io_conf.pull_up_en = 0;
    // gpio_config(&io_conf);

    ads131e0x_hal_reset_enable();
    io_conf.pin_bit_mask = ADS131E0X_RST_SEL;//reset pin
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    ads131e0x_hal_start_disable();
    io_conf.pin_bit_mask = ADS131E0X_START_SEL;//start pin
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    io_conf.pin_bit_mask = ADS131E0X_DRDY_SEL;//data ready pin
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_PIN_INTR_NEGEDGE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);
}

static int32_t spi_interface_init(void)
{   
    int32_t ret;

    ret = spi_mng_bus_init();
    if(ret < 0)
    {
        return -1;
    }

    spi_device_interface_config_t devcfg={
        .clock_speed_hz = 4000*1000, 
        .mode = 1, 
        .spics_io_num = ADS131E0X_SPI_CS_NUM, 
        .queue_size = 4, //We want to queue 4 transactions at a time
        .cs_ena_posttrans = 12
    };

    ret = spi_mng_bus_add_dev(&devcfg, &spi_ads131e0x);

    if(ret < 0)
    {
        return -2;
    }

    return 0;
}

// mutex may result to transmite failed, in that case, no corresponding value can be return.
// uint8_t ads131e0x_hal_read_write_byte(uint8_t tx_data)
// {
//     esp_err_t err;
//     spi_transaction_t trans;

//     memset(&trans, 0, sizeof(spi_transaction_t));
//     trans.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
//     trans.length = 8;             //Command is 8 bits
//     trans.tx_data[0] = tx_data;

//     ret = spi_mng_take(0);
//     if(ret < 0)
//     {
//         return 
//     }
//     err = spi_mng_device_polling_transmit(spi_ads131e0x, &trans);
//     assert(err==ESP_OK);

//     return trans.rx_data[0];
// }

int32_t ads131e0x_hal_read_write(uint8_t *tx_buf, uint8_t *rx_buf, uint32_t length, uint32_t timeout_ms)
{
    if((tx_buf == NULL) || (rx_buf == NULL))
    {
        return -1;
    }

    spi_transaction_t trans;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.flags = 0;
    trans.length = length * 8;
    trans.rx_buffer = rx_buf;
    trans.tx_buffer = tx_buf;

    int32_t ret = spi_mng_device_polling_transmit(spi_ads131e0x, &trans, timeout_ms);

    if(ret < -1)
    {
        //transmit failed
        return -2;
    }
    else if(ret < 0)
    {
        //time out
        return -3;
    }
    
    return 0;
}


int32_t ads131e0x_drdy_interrupt_enable(drdy_cb_t cb)
{
    esp_err_t err = gpio_isr_handler_add(ADS131E0X_DRDY_NUM, cb, (void*) ADS131E0X_DRDY_NUM);
    if(err != ESP_OK)
    {
        return -1;
    }

    return 0;
}

int32_t ads131e0x_drdy_interrupt_disable (void)
{
    esp_err_t err = gpio_isr_handler_remove(ADS131E0X_DRDY_NUM);
    if(err != ESP_OK)
    {
        return -1;
    }
    
    return 0;
}



// int32_t ads131e0x_read_reg(uint8_t head_reg, uint8_t num, uint8_t *reg_data)
// {
//     if(reg_data == NULL)
//     {
//         return -1;
//     }

//     uint32_t length = num + 2;

//     uint8_t *tx_buf = (uint8_t*)calloc(length, sizeof(uint8_t));
//     if(tx_buf == NULL)
//     {
//         return -2;
//     }

//     uint8_t *rx_buf = (uint8_t*)calloc(length, sizeof(uint8_t));
//     if(rx_buf == NULL)
//     {
//         return -3;
//     }

//     tx_buf[0] = (RREG | reg);
//     tx_buf[1] = (num - 1);

//     spi_transaction_t trans;

//     memset(&trans, 0, sizeof(spi_transaction_t));
//     trans.flags = 0;
//     trans.length = length * 8;
//     trans.rx_buffer = reg_data;
//     trans.tx_buffer = tx_buf;

//     esp_err_t ret = spi_mng_device_polling_transmit(spi_ads131e0x, &trans);
//     assert(ret==ESP_OK);

//     memcpy(reg_data, rx_buf + 2);

//     return 0;
// }

// int32_t ads131e0x_write_reg(uint8_t head_reg, uint8_t num, uint8_t *reg_data)
// {
//     if(reg_data == NULL)
//     {
//         return -1;
//     }

//     uint32_t length = num + 2;

//     uint8_t *tx_buf = (uint8_t*)calloc(length, sizeof(uint8_t));
//     if(tx_buf == NULL)
//     {
//         return -2;
//     }

//     tx_buf[0] = (WREG | reg);
//     tx_buf[1] = (num - 1);

//     memcpy(tx_buf+2, reg_data, num);

//     spi_transaction_t trans;

//     memset(&trans, 0, sizeof(spi_transaction_t));
//     trans.flags = 0;
//     trans.length = length * 8;
//     trans.rx_buffer = NULL;
//     trans.tx_buffer = tx_buf;

//     esp_err_t ret = spi_mng_device_polling_transmit(spi_ads131e0x, &trans);
//     assert(ret==ESP_OK);

//     return 0;
// }