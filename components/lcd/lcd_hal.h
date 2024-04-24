#ifndef __LCD_HAL_H
#define __LCD_HAL_H


#ifdef __cplusplus
extern "C" {
#endif


#include "../includes_common/includes_common.h"




//#define LCD_SPI_MISO_NUM        (25)
#define LCD_SPI_MOSI_NUM        (33)
#define LCD_SPI_SCLK_NUM        (32)
#define LCD_SPI_CS_NUM          (4)
#define LCD_SPI_RS_NUM          (2)

#define ADC_SPI_CS_NUM          (26)


#define LCD_LED_NUM             (13)
#define LCD_RST_NUM             (15)


#define LCD_LED_ON            (1ULL<<LCD_LED_NUM)
#define LCD_RST_SEL             (1ULL<<LCD_RST_NUM)

#define LCD_LED_OFF            (1ULL<<LCD_LED_NUM)

#define ADC_SPI_CS_select          (1ULL<<ADC_SPI_CS_NUM)

uint8_t volatile lcd_adc_mutex;


int32_t LCD_hal_init(void);

int32_t LCD_hal_poweron(void);
int32_t LCD_hal_poweroff(void);
// int32_t LCD_hal_CS_disable(void);
// int32_t LCD_hal_CS_enable(void);
int32_t LCD_hal_reset_enable(void);
int32_t LCD_hal_reset_disable(void);

int32_t LCD_hal_data_enable(void);
int32_t LCD_hal_cmd_enable(void);

int32_t LCD_hal_write_byte(uint8_t data);
int32_t LCD_hal_write_bytes(uint8_t *data, uint16_t length);



#ifdef __cplusplus
}
#endif

#endif
