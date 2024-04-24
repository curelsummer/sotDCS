#include "LCD.h"
#include "LCD_hal.h"
#include "start_bg.h"
#include "pic.h"
#include "font.h"
#include <string.h>


#define BT_ICON_WIDTH            16
#define BT_ICON_HEIGHT           24

#define BAT_ICON_WIDTH            35
#define BAT_ICON_HEIGHT           20 

#define DEVN_ICON_WIDTH           24
#define DEVN_ICON_HEIGHT          24

#define LCD_WIDTH                 320
#define LCD_HEIGHT                480


_lcd_dev lcddev;


//������ɫ,������ɫ
uint16_t POINT_COLOR = 0x0000,BACK_COLOR = 0xFFFF;  
uint16_t DeviceCode;	 

/*****************************************************************************
 * @name       :void LCD_WR_REG(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit command to the LCD screen
 * @parameters :data:Command value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_REG(uint8_t data)
{ 
//   LCD_hal_CS_enable();     
     LCD_hal_cmd_enable();	  
 //  SPI_WriteByte(SPI1,data);
   LCD_hal_write_byte(data);
 //  vTaskDelay(10/portTICK_RATE_MS);
 //  LCD_hal_CS_disable();	
}

/*****************************************************************************
 * @name       :void LCD_WR_DATA(u8 data)
 * @date       :2018-08-09 
 * @function   :Write an 8-bit data to the LCD screen
 * @parameters :data:data value to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WR_DATA(uint8_t data)
{
//   LCD_hal_CS_enable();
  LCD_hal_data_enable();
 //  SPI_WriteByte(SPI1,data);
   LCD_hal_write_byte(data);
 //  vTaskDelay(10/portTICK_RATE_MS);
//   LCD_hal_CS_disable();
}

/*****************************************************************************
 * @name       :void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
 * @date       :2018-08-09 
 * @function   :Write data into registers
 * @parameters :LCD_Reg:Register address
                LCD_RegValue:Data to be written
 * @retvalue   :None
******************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{	
    LCD_WR_REG(LCD_Reg);  
    LCD_WR_DATA(LCD_RegValue);	    		 
}	   

/*****************************************************************************
 * @name       :void LCD_WriteRAM_Prepare(void)
 * @date       :2018-08-09 
 * @function   :Write GRAM
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	 
void LCD_WriteRAM_Prepare(void)
{
    LCD_WR_REG(lcddev.wramcmd);
}	 

/*****************************************************************************
 * @name       :void Lcd_WriteData_16Bit(u16 Data)
 * @date       :2018-08-09 
 * @function   :Write an 16-bit command to the LCD screen
 * @parameters :Data:Data to be written
 * @retvalue   :None
******************************************************************************/	 
void Lcd_WriteData_16Bit(uint16_t Data)
{	
  //18Bit	
 // LCD_hal_CS_enable();
    LCD_WR_DATA((Data>>8)&0xF8);//RED
    LCD_WR_DATA((Data>>3)&0xFC);//GREEN
    LCD_WR_DATA(Data<<3);//BLUE
//	vTaskDelay(1/portTICK_RATE_MS);
    //LCD_hal_CS_disable();
}

/*****************************************************************************
 * @name       :void LCD_DrawPoint(u16 x,u16 y)
 * @date       :2018-08-09 
 * @function   :Write a pixel data at a specified location
 * @parameters :x:the x coordinate of the pixel
                y:the y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/	
void LCD_DrawPoint(uint16_t x,uint16_t y)
{
    LCD_SetCursor(x,y);
//	LCD_hal_CS_enable();
    Lcd_WriteData_16Bit(POINT_COLOR); 
//	 LCD_hal_CS_disable();
}

/*****************************************************************************
 * @name       :void LCD_Clear(u16 Color)
 * @date       :2018-08-09 
 * @function   :Full screen filled LCD screen
 * @parameters :color:Filled color
 * @retvalue   :None
******************************************************************************/	
void LCD_Clear(uint16_t Color)
{
  unsigned int i,m;  
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
//	LCD_hal_CS_enable();
    LCD_hal_reset_disable();
    for(i=0;i<lcddev.height;i++)
    {LCD_hal_data_enable();
    for(m=0;m<lcddev.width;m++)
    {	
            Lcd_WriteData_16Bit(Color);
        }
    }
//	 LCD_hal_CS_disable();
} 

void lcd_draw16(uint16_t x, uint16_t y, uint8_t *p)
{
    uint32_t i,m;  
    uint8_t buf[960];
    
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
    LCD_hal_reset_disable();
    LCD_hal_data_enable();
    for(i=0;i<lcddev.height*lcddev.width*2;)
    {
        for(m=0; m<960; m+=3,i+=2)
        {
            buf[m] = p[i+1]&0xF8;//RED
            buf[m+1] = ((p[i+1]<<5)|(p[i]>>3))&0xFC;//GREEN
            buf[m+2] = (p[i]<<3)&0xF8;//BLUE

        }   
        LCD_hal_write_bytes(buf, 960);
    }
//	 LCD_hal_CS_disable();
} 

void lcd_drawfull(const uint8_t *p)
{
    uint32_t i;  
    
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);   
    LCD_hal_reset_disable();
    LCD_hal_data_enable();

    for(i=0;i<460800;i+=960)
    {
        LCD_hal_write_bytes((uint8_t *)(p+i), 960);
    }
}

void lcd_draw_rect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    LCD_SetWindows(x,y,x+width-1,y+height-1);
    LCD_hal_reset_disable();
    LCD_hal_data_enable();

    uint8_t buf[320*3];

    uint8_t red = (color >> 16) & 0xff;
    uint8_t green = (color >> 8) & 0xff;
    uint8_t blue = color & 0xff;

    uint8_t *ptr = buf;
    for(uint32_t px=0; px<width; px++)
    {
        *ptr++ = red;
        *ptr++ = green;
        *ptr++ = blue;
    }

    for(uint32_t i=0; i<height; i++)
    {
        LCD_hal_write_bytes(buf, width*3);
    }
}

void lcd_show_achar16(uint16_t x,uint16_t y, uint8_t num, uint8_t mode)
{
    static uint8_t buf[16*8*3];

    uint8_t fcolor = (mode == 0)?0xff:0x00;
    uint8_t bcolor = ~fcolor;

    uint8_t *bufptr = buf;
    uint8_t *ascptr = asc_1608[num];


    for(uint32_t i=0; i<16; i++)
    {
        uint8_t tmp = *ascptr++;
        for(uint32_t j=0; j<8; j++)
        {
            if(tmp&0x80)
            {
                *bufptr++ = fcolor;
                *bufptr++ = fcolor;
                *bufptr++ = fcolor;
            }            
            else
            {
                *bufptr++ = bcolor;
                *bufptr++ = bcolor;
                *bufptr++ = bcolor;
            }
            tmp<<=1;
        }
    }

    LCD_SetWindows(x,y,x+8-1,y+16-1);
    LCD_hal_data_enable();
    LCD_hal_write_bytes(buf, sizeof(buf));
}

void lcd_show_achar32(uint16_t x,uint16_t y, uint8_t num, uint8_t mode, const uint8_t *bg_pic)
{
    uint8_t temp;
    uint32_t pos=0,i,j,k,m;
    
    static uint8_t buf[32*16*3];
    uint8_t *ptr = asc_3216[num- ' '];

    LCD_SetWindows(x,y,x+16-1,y+32-1);
    if(!mode) 
    {		
        for(i=0; i<32*2; i++)
        {
            temp = *ptr++;
            k = ((i%4) * 16 * 3 + i/4);
            for(j=0; j<8; j++,k+=3*16)
            {
                if(temp&0x80)
                {
                    buf[k] = 0xFF;
                    buf[k+1] = 0xFF;
                    buf[k+2] = 0xFF;
                }            
                else
                {
                    m = ((x+i%2+j) + (y+i/2)*320)*3;
                    buf[k] = bg_pic[m];
                    buf[k+1] = bg_pic[m+1];
                    buf[k+2] = bg_pic[m+2];
                }
                temp<<=1; 
            }
        }
    }
    else
    {
        for(i=0,k=0; i<32*2; i++)
        {
            temp = *ptr++;
            for(j=0; j<8; j++,k+=3)
            {
                if(temp&0x80)
                {
                    buf[k] = 0xFF;
                    buf[k+1] = 0xFF;
                    buf[k+2] = 0xFF;
                }            
                else
                {
                    buf[k] = 0x00;
                    buf[k+1] = 0x00;
                    buf[k+2] = 0xFF;
                }
                temp<<=1; 
            }
        }
    }
    LCD_hal_data_enable();
    LCD_hal_write_bytes(buf, sizeof(buf));
}

void lcd_show_cchar24(uint16_t x,uint16_t y, uint16_t index, uint8_t mode, const uint8_t *bg_pic)
{  
    uint8_t temp;
    uint32_t pos=0,i,j,k,m;   
    
    if(index > 32)
    {//no cchar
        return;
    }

    static uint8_t buf[24*24*3];
    index = index - 1;
    LCD_SetWindows(x,y,x+24-1,y+24-1);
    if(!mode) 
    {
        for(i=0,k=0; i<24*3; i++)
        {
            temp=tfont_2424[index][pos++];
            for(j=0; j<8; j++,k+=3)
            {
                if(temp&0x80)
                {
                    buf[k] = 0xFF;
                    buf[k+1] = 0xFF;
                    buf[k+2] = 0xFF;
                }            
                else
                {                      
                    m = ((x+i%3+j) + (y+i/3)*320)*3;
                    buf[k] = bg_pic[m];
                    buf[k+1] = bg_pic[m+1];
                    buf[k+2] = bg_pic[m+2];		
                }
                temp<<=1; 
            }
        }
        LCD_hal_data_enable();
        LCD_hal_write_bytes(buf, sizeof(buf));
    }
    else
    {
        for(i=0,k=0; i<24*3; i++)
        {
            temp=tfont_2424[index][pos++];
            for(j=0; j<8; j++,k+=3)
            {
                if(temp&0x80)
                {
                    buf[k] = 0xFF;
                    buf[k+1] = 0xFF;
                    buf[k+2] = 0xFF;		 
                }            
                else
                {
                    buf[k] = 0x00;
                    buf[k+1] = 0x00;
                    buf[k+2] = 0xFF;		  	
                }
                temp<<=1; 
            }
        }
        
        LCD_hal_data_enable();
        LCD_hal_write_bytes(buf, sizeof(buf));

    }
}

void lcd_show_achar48(uint16_t x,uint16_t y, uint8_t num, uint8_t mode, const uint8_t *bg_pic)
{  
    uint8_t temp;
    uint32_t pos=0,i,j,k,m;   
    
    static uint8_t buf[48*24*3];
    
    uint8_t *ptr = asc_4824[num-' '];

    LCD_SetWindows(x,y,x+48/2-1,y+48-1);
    if(!mode) 
    {		
        for(i=0,k=0; i<48*3; i++)
        {
            temp = *ptr++;
            for(j=0; j<8; j++,k+=3)
            {
                if(temp&0x80)
                {
                    buf[k] = 0xFF;
                    buf[k+1] = 0xFF;
                    buf[k+2] = 0xFF;
                }            
                else
                {
                    m = ((x+i%3+j) + (y+i/3)*320)*3;
                    buf[k] = bg_pic[m];
                    buf[k+1] = bg_pic[m+1];
                    buf[k+2] = bg_pic[m+2];
                }
                temp<<=1; 
            }
        }
    }
    else
    {
        for(i=0,k=0; i<48*3; i++)
        {
            temp = *ptr++;
            for(j=0; j<8; j++,k+=3)
            {
                if(temp&0x80)
                {
                    buf[k] = 0xFF;
                    buf[k+1] = 0xFF;
                    buf[k+2] = 0xFF;
                }            
                else
                {
                    buf[k] = 0x00;
                    buf[k+1] = 0x00;
                    buf[k+2] = 0xFF;
                }
                temp<<=1; 
            }
        }
    }
    LCD_hal_data_enable();
    LCD_hal_write_bytes(buf, sizeof(buf));
}

void lcd_show_cchar48(uint16_t x,uint16_t y, uint16_t num, uint8_t mode, const uint8_t *bg_pic)
{  
    uint8_t temp;
    uint32_t pos=0,i,j,k,m,p;   
    
    if(num > 21)
    {//no cchar
        return;
    }

    static uint8_t buf[48*20*3];
    num = num - 1;
    LCD_SetWindows(x,y,x+40-1,y+48-1);
    if(!mode) 
    {
        for(p=0;p<2;p++)
        {
            for(i=0,k=0; i<40*3; i++)
            {
                temp=tfont4848[num][pos++];
                for(j=0; j<8; j++,k+=3)
                {
                    if(temp&0x80)
                    {
                        buf[k] = 0xFF;
                        buf[k+1] = 0xFF;
                        buf[k+2] = 0xFF;
                    }            
                    else
                    {                      
                        m = ((x+i%3+j) + (y+i/3)*320)*3;
                        buf[k] = bg_pic[m];
                        buf[k+1] = bg_pic[m+1];
                        buf[k+2] = bg_pic[m+2];		
                    }
                    temp<<=1; 
                }
            }
            LCD_hal_data_enable();
            LCD_hal_write_bytes(buf, sizeof(buf));
        }
    }
    else
    {
        for(p=0;p<2;p++)
        {
            for(i=0,k=0; i<40*3; i++)
            {
                temp=tfont4848[num][pos++];
                for(j=0; j<8; j++,k+=3)
                {
                    if(temp&0x80)
                    {
                        buf[k] = 0xFF;
                        buf[k+1] = 0xFF;
                        buf[k+2] = 0xFF;		 
                    }            
                    else
                    {
                        buf[k] = 0x00;
                        buf[k+1] = 0x00;
                        buf[k+2] = 0xFF;		  	
                    }
                    temp<<=1; 
                }
            }
            
            LCD_hal_data_enable();
            LCD_hal_write_bytes(buf, sizeof(buf));
        }
    }
}

int32_t lcd_show_str16(uint16_t x, uint16_t y, char *data, uint8_t sel)
{
    if(data == NULL)
    {
        return -1;
    }

    uint32_t len = strlen(data);
    for(int32_t i=0; i<len; i++)
    {
        lcd_show_achar16(x+8*i, y, data[i], sel);
    }

    return 0;
}

int32_t lcd_show_str32(uint16_t x, uint16_t y, char *data, uint8_t sel,  const uint8_t *bg_pic)
{
    if(data == NULL)
    {
        return -1;
    }

    uint32_t len = strlen(data);
    for(int32_t i=0; i<len; i++)
    {
        lcd_show_achar32(x+16*i, y, data[i], sel, bg_pic);
    }

    return 0;
}

int32_t lcd_show_str(uint16_t x, uint16_t y, char *data, uint8_t sel,  const uint8_t *bg_pic)
{
    if(data == NULL)
    {
        return -1;
    }

    for(int32_t i=0; i<strlen(data); i++)
    {
        lcd_show_achar48(x+24*i, y, data[i], sel, bg_pic);
    }

    return 0;
}

static uint32_t cstrlen(uint16_t *cstr)
{
    uint16_t *ptr = cstr;
    uint32_t i = 0;

    while(*ptr != 0)
    {
        i++;
        ptr++;
    }

    return i;
}


int32_t lcd_show_cstr24(uint16_t x,uint16_t y, uint16_t *data, uint8_t sel,const uint8_t *bg_pic)
{  
    if(data == NULL)
    {
        return -1;
    }
    
    for(int32_t i=0; i<cstrlen(data); i++)
    {
        lcd_show_cchar24(x+24*i, y, data[i], sel, bg_pic);
    }

    return 0;
}

int32_t lcd_show_cstr(uint16_t x,uint16_t y, uint16_t *data, uint8_t sel,const uint8_t *bg_pic)
{  
    if(data == NULL)
    {
        return -1;
    }
    
    for(int32_t i=0; i<cstrlen(data); i++)
    {
        lcd_show_cchar48(x+40*i, y, data[i], sel, bg_pic);
    }

    return 0;
}



void lcd_disp_part_pic(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint8_t *pic)
{
    LCD_SetWindows(x,y,x+width-1,y+height-1);
    LCD_hal_data_enable();

    
    for(uint32_t i=0; i<height; i++)
    {
        uint8_t *ptr = pic + i * 320 * 3 + x;
        LCD_hal_write_bytes(ptr, width * 3);
    }
}

void lcd_show_bat(uint16_t x, uint16_t y, const uint8_t *fg_pic, const uint8_t *bg_pic)
{
    uint8_t temp[3];
    uint32_t i,j,k,m;
    
    static uint8_t buf[BAT_ICON_WIDTH*BAT_ICON_HEIGHT*3];
    
    LCD_SetWindows(x,y,x+BAT_ICON_WIDTH-1,y+BAT_ICON_HEIGHT-1);

    for(i=0,k=0; i<BAT_ICON_HEIGHT; i++)
    {
        for(j=0; j<BAT_ICON_WIDTH; j++,k+=3)
        {
            temp[0] = fg_pic[i*BAT_ICON_WIDTH*3 + j*3];
            temp[1] = fg_pic[i*BAT_ICON_WIDTH*3 + j*3+1];
            temp[2] = fg_pic[i*BAT_ICON_WIDTH*3 + j*3+2];
            
            if(temp[0]!=0xFF || temp[1]!=0xFF || temp[2]!=0xFF)
            {
                buf[k] = temp[0];
                buf[k+1] = temp[1];
                buf[k+2] = temp[2];
            }
            else
            {
                m = ((x+j) + (y+i)*LCD_WIDTH)*3;
                buf[k] = bg_pic[m];
                buf[k+1] = bg_pic[m+1];
                buf[k+2] = bg_pic[m+2];
            }
        }
    }
    
    LCD_hal_data_enable();
    LCD_hal_write_bytes(buf, sizeof(buf));
}

void lcd_show_devn(uint16_t x, uint16_t y, const uint8_t *fg_pic, const uint8_t *bg_pic)
{
    uint8_t temp[3];
    uint32_t i,j,k,m;
    
    static uint8_t buf[DEVN_ICON_WIDTH*DEVN_ICON_HEIGHT*3];
    
    LCD_SetWindows(x,y,x+DEVN_ICON_WIDTH-1,y+DEVN_ICON_HEIGHT-1);

    for(i=0,k=0; i<DEVN_ICON_HEIGHT; i++)
    {
        for(j=0; j<DEVN_ICON_WIDTH; j++,k+=3)
        {
            temp[0] = fg_pic[i*DEVN_ICON_WIDTH*3 + j*3];
            temp[1] = fg_pic[i*DEVN_ICON_WIDTH*3 + j*3+1];
            temp[2] = fg_pic[i*DEVN_ICON_WIDTH*3 + j*3+2];
            
            if(temp[0]!=0xFF || temp[1]!=0xFF || temp[2]!=0xFF)
            {
                buf[k] = temp[0];
                buf[k+1] = temp[1];
                buf[k+2] = temp[2];
            }
            else
            {
                m = ((x+j) + (y+i)*LCD_WIDTH)*3;
                buf[k] = bg_pic[m];
                buf[k+1] = bg_pic[m+1];
                buf[k+2] = bg_pic[m+2];
            }
        }
    }
    
    LCD_hal_data_enable();
    LCD_hal_write_bytes(buf, sizeof(buf));
}


void lcd_backlight_on(void)
{
    LCD_hal_poweron();
}

void lcd_backlight_off(void)
{
    LCD_hal_poweroff();
}


// void lcd_show_bt(uint16_t x, uint16_t y, const uint8_t *fg_pic, const uint8_t *bg_pic)
// {
//     uint8_t temp[3];
//     uint32_t i,j,k,m;
    
//     static uint8_t buf[BT_ICON_WIDTH*BT_ICON_HEIGHT*3];
    
//     LCD_SetWindows(x,y,x+BT_ICON_WIDTH-1,y+BT_ICON_HEIGHT-1);

//     for(i=0,k=0; i<BT_ICON_HEIGHT; i++)
//     {
//         for(j=0; j<BT_ICON_WIDTH; j++,k+=3)
//         {
//             temp[0] = fg_pic[i*BT_ICON_WIDTH*3 + j*3];
//             temp[1] = fg_pic[i*BT_ICON_WIDTH*3 + j*3+1];
//             temp[2] = fg_pic[i*BT_ICON_WIDTH*3 + j*3+2];
            
//             if(temp[0]!=0xFF || temp[1]!=0xFF || temp[2]!=0xFF)
//             {
//                 buf[k] = temp[0];
//                 buf[k+1] = temp[1];
//                 buf[k+2] = temp[2];
//             }            
//             else
//             {
//                 m = ((x+j) + (y+i)*LCD_WIDTH)*3;
//                 buf[k] = bg_pic[m];
//                 buf[k+1] = bg_pic[m+1];
//                 buf[k+2] = bg_pic[m+2];
//             }
//         }
//     }
    
//     LCD_hal_data_enable();
//     LCD_hal_write_bytes(buf, sizeof(buf));
// }

void lcd_show_bt(uint16_t x, uint16_t y, const uint8_t *bg_pic,uint8_t bt_flag)
{
    uint8_t temp;
    uint32_t pos=0,i,j,k,m; 
    
    static uint8_t buf[BT_ICON_WIDTH*BT_ICON_HEIGHT*3];
    
    LCD_SetWindows(x,y,x+BT_ICON_WIDTH-1,y+BT_ICON_HEIGHT-1);
    for(i=0,k=0; i<24*2; i++)
    {
        temp=pic_bt[pos++];
        for(j=0; j<8; j++,k+=3)
        {
            if(temp&0x80)
            {
                if(bt_flag == 0)
                {
                    buf[k] = 0x33;
                    buf[k+1] = 0x33;
                    buf[k+2] = 0x33;
                }
                else 
                {
                    buf[k] = 0x00;
                    buf[k+1] = 0x00;
                    buf[k+2] = 0xFF;
                }
            }
            else
            {
                m = ((x+i%2+j) + (y+i/2)*320)*3;
                buf[k] = bg_pic[m];
                buf[k+1] = bg_pic[m+1];
                buf[k+2] = bg_pic[m+2];
            }
            temp<<=1; 
        }
    }
    
    LCD_hal_data_enable();
    LCD_hal_write_bytes(buf, sizeof(buf));
}


/*****************************************************************************
 * @name       :void LCD_RESET(void)
 * @date       :2018-08-09 
 * @function   :Reset LCD screen
 * @parameters :None
 * @retvalue   :None
******************************************************************************/	
void LCD_RESET(void)
{
//LCD_hal_CS_enable();
    // LCD_hal_reset_disable();
    //    vTaskDelay(pdMS_TO_TICKS(50));

    LCD_hal_reset_enable();
    vTaskDelay(pdMS_TO_TICKS(10));	
    LCD_hal_reset_disable();
    vTaskDelay(pdMS_TO_TICKS(100));
}

     
int32_t lcd_init(void)
{  
    int32_t ret;
    ret = LCD_hal_init();//LCD GPIO��ʼ��	
    if(ret < 0)
    {
        return -1;
    }

    LCD_RESET(); //LCD ��λ
    //printf("LCD_hal_init\n");
     
//************* ILI9488��ʼ��**********//	
 //      LCD_hal_CS_enable();
    LCD_WR_REG(0XF7);
    LCD_WR_DATA(0xA9);
    LCD_WR_DATA(0x51);
    LCD_WR_DATA(0x2C);
    LCD_WR_DATA(0x82);
    LCD_WR_REG(0xC0);
    LCD_WR_DATA(0x11);
    LCD_WR_DATA(0x09);
    LCD_WR_REG(0xC1);
    LCD_WR_DATA(0x41);
    LCD_WR_REG(0XC5);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x80);
    LCD_WR_REG(0xB1);
    LCD_WR_DATA(0xB0);
    LCD_WR_DATA(0x11);
    LCD_WR_REG(0xB4);
    LCD_WR_DATA(0x02);
    LCD_WR_REG(0xB6);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x42);
    LCD_WR_REG(0xB7);
    LCD_WR_DATA(0xc6);
    LCD_WR_REG(0xBE);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x04);
    LCD_WR_REG(0xE9);
    LCD_WR_DATA(0x00);
    LCD_WR_REG(0x36);
    LCD_WR_DATA((1<<3)|(0<<7)|(1<<6)|(1<<5));
    LCD_WR_REG(0x3A);
    LCD_WR_DATA(0x66);
    LCD_WR_REG(0xE0);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x07);
    LCD_WR_DATA(0x10);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x0B);
    LCD_WR_DATA(0x41);
    LCD_WR_DATA(0x89);
    LCD_WR_DATA(0x4B);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x0C);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x18);
    LCD_WR_DATA(0x1B);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0XE1);
    LCD_WR_DATA(0x00);
    LCD_WR_DATA(0x17);
    LCD_WR_DATA(0x1A);
    LCD_WR_DATA(0x04);
    LCD_WR_DATA(0x0E);
    LCD_WR_DATA(0x06);
    LCD_WR_DATA(0x2F);
    LCD_WR_DATA(0x45);
    LCD_WR_DATA(0x43);
    LCD_WR_DATA(0x02);
    LCD_WR_DATA(0x0A);
    LCD_WR_DATA(0x09);
    LCD_WR_DATA(0x32);
    LCD_WR_DATA(0x36);
    LCD_WR_DATA(0x0F);
    LCD_WR_REG(0x11);
    //delay_ms(120);
    //vTaskDelay(300/portTICK_RATE_MS);
    LCD_WR_REG(0x29);
//	LCD_hal_CS_disable();
    
    LCD_direction(USE_HORIZONTAL);

    lcd_drawfull(pic_back);
    
    LCD_hal_poweron();
    //vTaskDelay(pdMS_TO_TICKS(100));
    // lcd_drawfull(pic_dc);
    return 0;
}

/*****************************************************************************
 * @name       :void LCD_SetWindows(u16 xStar, u16 yStar,u16 xEnd,u16 yEnd)
 * @date       :2018-08-09 
 * @function   :Setting LCD display window
 * @parameters :xStar:the bebinning x coordinate of the LCD display window
                                yStar:the bebinning y coordinate of the LCD display window
                                xEnd:the endning x coordinate of the LCD display window
                                yEnd:the endning y coordinate of the LCD display window
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
{	
//LCD_hal_CS_enable();
    LCD_WR_REG(lcddev.setxcmd);	
    LCD_WR_DATA(xStar>>8);
    LCD_WR_DATA(0x00FF&xStar);		
    LCD_WR_DATA(xEnd>>8);
    LCD_WR_DATA(0x00FF&xEnd);

    LCD_WR_REG(lcddev.setycmd);	
    LCD_WR_DATA(yStar>>8);
    LCD_WR_DATA(0x00FF&yStar);		
    LCD_WR_DATA(yEnd>>8);
    LCD_WR_DATA(0x00FF&yEnd);

    LCD_WriteRAM_Prepare();
    //LCD_hal_CS_disable();
}

/*****************************************************************************
 * @name       :void LCD_SetCursor(u16 Xpos, u16 Ypos)
 * @date       :2018-08-09 
 * @function   :Set coordinate value
 * @parameters :Xpos:the  x coordinate of the pixel
                                Ypos:the  y coordinate of the pixel
 * @retvalue   :None
******************************************************************************/ 
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
    LCD_SetWindows(Xpos,Ypos,Xpos,Ypos);
} 

/*****************************************************************************
 * @name       :void LCD_direction(u8 direction)
 * @date       :2018-08-09 
 * @function   :Setting the display direction of LCD screen
 * @parameters :direction:0-0 degree
                          1-90 degree
                                                    2-180 degree
                                                    3-270 degree
 * @retvalue   :None
******************************************************************************/ 
void LCD_direction(uint8_t direction)
{ 

//	  LCD_hal_CS_enable();
            lcddev.setxcmd=0x2A;
            lcddev.setycmd=0x2B;
            lcddev.wramcmd=0x2C;
    switch(direction){		  
        case 0:						 	 		
            lcddev.width=LCD_W;
            lcddev.height=LCD_H;		
            LCD_WriteReg(0x36,(1<<3)|(0<<6)|(0<<7));//BGR==1,MY==0,MX==0,MV==0
        break;
        case 1:
            lcddev.width=LCD_H;
            lcddev.height=LCD_W;
            LCD_WriteReg(0x36,(1<<3)|(0<<7)|(1<<6)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
        break;
        case 2:						 	 		
            lcddev.width=LCD_W;
            lcddev.height=LCD_H;	
            LCD_WriteReg(0x36,(1<<3)|(1<<6)|(1<<7));//BGR==1,MY==0,MX==0,MV==0
        break;
        case 3:
            lcddev.width=LCD_H;
            lcddev.height=LCD_W;
            LCD_WriteReg(0x36,(1<<3)|(1<<7)|(1<<5));//BGR==1,MY==1,MX==0,MV==1
        break;	
        default:break;
    }

//	LCD_hal_CS_disable();
}









