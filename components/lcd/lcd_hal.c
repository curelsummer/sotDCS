#include "LCD_hal.h"
#include "spi_mng.h"


static spi_device_handle_t spi_lcd;


int32_t LCD_hal_poweron(void)
{
    gpio_set_level(LCD_LED_NUM, 0);
    return 0;
}

int32_t LCD_hal_poweroff(void)
{
    gpio_set_level(LCD_LED_NUM, 1);
    return 0;
}
/*
int32_t LCD_hal_CS_enable(void)
{
    gpio_set_level(LCD_SPI_CS_NUM, 0);
    return 0;
}

int32_t LCD_hal_CS_disable(void)
{
    gpio_set_level(LCD_SPI_CS_NUM, 1);
    return 0;
}
*/
int32_t LCD_hal_reset_enable(void)
{
    gpio_set_level(LCD_RST_NUM, 0);
    return 0;
}

int32_t LCD_hal_reset_disable(void)
{
    gpio_set_level(LCD_RST_NUM, 1);
    return 0;
}


int32_t LCD_hal_cmd_enable(void)
{
    gpio_set_level(LCD_SPI_RS_NUM, 0);
    return 0;
}

int32_t LCD_hal_data_enable(void)
{
    gpio_set_level(LCD_SPI_RS_NUM, 1);
    return 0;
}


static int32_t lcd_spi_interface_init(void)
{
   int32_t ret;

    ret = spi_mng_bus_init();
    if(ret < 0)
    {
        return -1;
    }

    spi_device_interface_config_t devcfg={
        .clock_speed_hz = 20000*1000, 
        .mode = 0, 
        .spics_io_num = LCD_SPI_CS_NUM, 
        .queue_size = 4096, //We want to be able to queue 4 transactions at a time
        .cs_ena_posttrans = 10
    };

    ret = spi_mng_bus_add_dev(&devcfg, &spi_lcd);

    if(ret < 0)
    {
        return -2;
    }

    return 0;
}


int32_t LCD_hal_init(void )
{
    gpio_config_t io_conf;  
	
    LCD_hal_cmd_enable();
    io_conf.pin_bit_mask = 1ULL<<LCD_SPI_RS_NUM;//
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
	
    LCD_hal_poweroff();
    io_conf.pin_bit_mask = LCD_LED_ON;//led pin
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    LCD_hal_reset_enable();
    io_conf.pin_bit_mask = LCD_RST_SEL;//reset pin
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf); 


    int32_t ret = lcd_spi_interface_init();
    if(ret < 0)
    {
        return -1;
    }
	
    //LCD_hal_poweron();
    
    return 0;
}

int32_t LCD_hal_write_byte(uint8_t data)
{
    
    spi_transaction_t trans;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.flags = SPI_TRANS_USE_TXDATA;
    trans.length = 8;             //Command is 8 bits
    trans.tx_data[0] = data;

    return spi_mng_device_polling_transmit(spi_lcd, &trans, 1000);
}

int32_t LCD_hal_write_bytes(uint8_t *data, uint16_t length)
{
    if(data == NULL)
    {
        printf("null pointer error in LCD_readwrite_regs.\r\n");
        return -1;
    }

    spi_transaction_t trans;

    memset(&trans, 0, sizeof(spi_transaction_t));
    trans.flags = 0;
    trans.length = length*8;
    trans.tx_buffer = data;


    int32_t ret;
send:    
    ret = spi_mng_device_polling_transmit(spi_lcd, &trans, 1000);

    if(ret < -1)
    {
        //transmit failed
        printf("spi_mng_device_polling_transmit err: %d\n", ret);
        return -2;
        //goto send;
    }
    else if(ret < 0)
    {
        //time out
        printf("spi_mng_device_polling_transmit err: %d\n", ret);
        goto send;
       // return -3;
    }
    
    return 0;
}



