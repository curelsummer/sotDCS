#ifndef __LCD_H
#define __LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../includes_common/includes_common.h"
#include "pic.h"


typedef struct  
{										    
	uint16_t width;			
	uint16_t height;		
	uint16_t id;			
	uint8_t  dir;			
	uint16_t	 wramcmd;		
	uint16_t  setxcmd;		
	uint16_t  setycmd;		
}_lcd_dev; 	


extern _lcd_dev lcddev;	

#define USE_HORIZONTAL  	2

#define LCD_W 320
#define LCD_H 480

extern uint16_t  POINT_COLOR; 
extern uint16_t  BACK_COLOR; 



#define WHITE            0xFFFF
#define BLACK      	     0x0000	  
#define BLUE       	     0x001F  
#define BRED             0XF81F
#define GRED 		     0XFFE0
#define GBLUE		     0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN 			 0XBC40 
#define BRRED 		     0XFC07 
#define GRAY  			 0X8430 


#define DARKBLUE      	 0X01CF	
#define LIGHTBLUE      	 0X7D7C	
#define GRAYBLUE       	 0X5458 


#define LIGHTGREEN       0X841F 
#define LIGHTGRAY        0XEF5B 
#define LGRAY 			 0XC618 
#define LGRAYBLUE        0XA651 
#define LBBLUE           0X2B12 




int32_t lcd_init(void);
void lcd_backlight_on(void);
void lcd_backlight_off(void);

void LCD_DisplayOn(void);
void LCD_DisplayOff(void);
void LCD_Clear(uint16_t Color);	 
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos);
void LCD_DrawPoint(uint16_t x,uint16_t y);
uint16_t  LCD_ReadPoint(uint16_t x,uint16_t y);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void LCD_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);		   
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd);

uint16_t LCD_RD_DATA(void);				    
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue);
void LCD_WR_DATA(uint8_t data);
uint16_t LCD_ReadReg(uint8_t LCD_Reg);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(uint16_t RGB_Code);
uint16_t LCD_ReadRAM(void);		   
uint16_t LCD_BGR2RGB(uint16_t c);
void LCD_SetParam(void);
void Lcd_WriteData_16Bit(uint16_t Data);
void LCD_direction(uint8_t direction );





void lcd_drawfull(const uint8_t *p);
void lcd_disp_part_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic);
void lcd_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color);
int32_t lcd_show_str16(uint16_t x, uint16_t y, char *data, uint8_t sel);
int32_t lcd_show_str32(uint16_t x, uint16_t y, char *data, uint8_t sel,  const uint8_t *bg_pic);
int32_t lcd_show_str(uint16_t x, uint16_t y, char *data, uint8_t sel,  const uint8_t *bg_pic);
int32_t lcd_show_cstr(uint16_t x,uint16_t y, uint16_t *data, uint8_t sel,const uint8_t *bg_pic);
int32_t lcd_show_cstr24(uint16_t x,uint16_t y, uint16_t *data, uint8_t sel,const uint8_t *bg_pic);

void lcd_show_bat(uint16_t x, uint16_t y, const uint8_t *fg_pic, const uint8_t *bg_pic);
void lcd_show_bt(uint16_t x, uint16_t y, const uint8_t *bg_pic,uint8_t bt_flag);
void lcd_show_devn(uint16_t x, uint16_t y, const uint8_t *fg_pic, const uint8_t *bg_pic);


void lcd_show_achar32(uint16_t x,uint16_t y, uint8_t num, uint8_t mode, const uint8_t *bg_pic);

// void lcd_disp_mode(uint8_t line, uint8_t *pic);
// void lcd_disp_cur(uint8_t line, uint8_t *pic);
// void lcd_disp_time(uint8_t line, uint8_t *pic);
// void lcd_disp_freq(uint8_t line, uint8_t *pic);

#ifdef __cplusplus
}
#endif

#endif

