//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//����Ӳ������Ƭ��STM32F407ZGT6,����ԭ��Explorer STM32F4������,��Ƶ168MHZ������12MHZ
//QDtech-TFTҺ������ for STM32 IOģ��
//xiao��@ShenZhen QDtech co.,LTD
//��˾��վ:www.qdtft.com
//�Ա���վ��http://qdtech.taobao.com
//wiki������վ��http://www.lcdwiki.com
//��˾�ṩ����֧�֣��κμ������⻶ӭ��ʱ����ѧϰ
//�̻�(����) :+86 0755-23594567 
//�ֻ�:15989313508���빤�� 
//����:lcdwiki01@gmail.com    support@lcdwiki.com    goodtft@163.com 
//����֧��QQ:3002773612  3002778157
//��������QQȺ:324828016
//��������:2018/08/09
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������ȫ�����Ӽ������޹�˾ 2018-2028
//All rights reserved
/****************************************************************************************************
//=========================================��Դ����================================================//
//     LCDģ��                STM32��Ƭ��
//      VCC          ��        DC5V/3.3V      //��Դ
//      GND          ��          GND          //��Դ��
//=======================================Һ���������߽���==========================================//
//��ģ��Ĭ��������������ΪSPI����
//     LCDģ��                STM32��Ƭ��    
//    SDI(MOSI)      ��          PB5          //Һ����SPI��������д�ź�
//    SDO(MISO)      ��          PB4          //Һ����SPI�������ݶ��źţ��������Ҫ�������Բ�����
//=======================================Һ���������߽���==========================================//
//     LCDģ�� 					      STM32��Ƭ�� 
//       LED         ��          PB13         //Һ������������źţ��������Ҫ���ƣ���5V��3.3V
//       SCK         ��          PB3          //Һ����SPI����ʱ���ź�
//      DC/RS        ��          PB14         //Һ��������/��������ź�
//       RST         ��          PB12         //Һ������λ�����ź�
//       CS          ��          PB15         //Һ����Ƭѡ�����ź�
//=========================================������������=========================================//
//���ģ�鲻���������ܻ��ߴ��д������ܣ����ǲ���Ҫ�������ܣ�����Ҫ���д���������
//	   LCDģ��                STM32��Ƭ�� 
//      T_IRQ        ��          PB1          //�����������ж��ź�
//      T_DO         ��          PB2          //������SPI���߶��ź�
//      T_DIN        ��          PF11         //������SPI����д�ź�
//      T_CS         ��          PC5          //������Ƭѡ�����ź�
//      T_CLK        ��          PB0          //������SPI����ʱ���ź�
**************************************************************************************************/	
 /* @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, QD electronic SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
**************************************************************************************************/	
#include "lcd.h"
#include "gui.h"
#include "test.h"
#include "pic.h"
#include "math.h"
#include "stdio.h"

//========================variable==========================//
uint16_t ColorTab[5]={RED,GREEN,BLUE,YELLOW,BRED};//������ɫ����
char str_power[3];
char dianliu[5];
char dianzu[5];

//=====================end of variable======================//

/*****************************************************************************
 * @name       :void DrawTestPage(uint8_t *str)
 * @date       :2018-08-09 
 * @function   :Drawing test interface
 * @parameters :str:the start address of the Chinese and English strings
 * @retvalue   :None
******************************************************************************/ 
void DrawTestPage(char *str)
{
//���ƹ̶���up
LCD_Clear(BLUE);
LCD_Fill(0,0,lcddev.width,20,BLUE);
//���ƹ̶���down
LCD_Fill(0,lcddev.height-20,lcddev.width,lcddev.height,BLUE);
POINT_COLOR=WHITE;
Gui_StrCenter(0,2,WHITE,BLUE,str,16,1);//������ʾ
//Gui_StrCenter(0,lcddev.height-18,WHITE,BLUE,"http://www.lcdwiki.com",16,1);//������ʾ
//���Ʋ�������
//LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
}

/*****************************************************************************
 * @name       :void main_test(void)
 * @date       :2018-08-09 
 * @function   :Drawing the main Interface of the Comprehensive Test Program
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void LCD_main_test(void)
{  
    //   printf("maintest_start\n");

//	DrawTestPage("深圳");	
//	Gui_StrCenter(0,30,RED,BLUE,"深",32,1);//������ʾ
//	Gui_StrCenter(0,60,RED,BLUE,"圳",32,1);//������ʾ
//显示电量
//LCD_battle(50);
// Show_Str(0,0,WHITE,BLUE,"123",14,1);
// Show_Str(50,0,WHITE,BLUE,"000",16,1);
// Show_Str(25,0,WHITE,BLUE,"123",32,1);

//显示电量
Battle_Power(80);


dianliu[0]=0x32;
dianliu[1]=0x30;
dianliu[2]=0x30;

uint8_t dian = StrToInt_8(dianliu);

dian = dian+22;
IntToStr_8(dianliu,dian);



ImpToStr(dianzu,5784);
//显示模式、显示电流、显示脉宽、显示间歇、显示时间、显示阻抗、显示电流
Show_Data(dianliu,"1500","85","1","10",dianzu,"1500");





//Show_Str(100,50,WHITE,BLUE," ",32,1);
//Show_Str(110,50,WHITE,BLUE,"8",32,1);



//绘制电量
	// int i,j;
	// while(1){
	// 	for(i=0;i<40;i++){
	// 		for(j=0;j<18;j++){
	// 			GUI_DrawPoint(i+170,j+12,WHITE);
	// 		}
	// }
	// Show_Str(178,13,BLUE,BLUE,"~",32,1);
	// 	for(i=38;i>19;i--){
	// 		for(j=0;j<16;j++){
	// 			GUI_DrawPoint(i+170,j+13,BLUE);
	// 		}
	// 	}
	// }
//数字显示电池电量
//Show_Str(178,13,BLUE,BLUE,"100",32,1);

//Show_Str(188,13,BLUE,BLUE,"~"+1,32,1);






	//Gui_StrCenter(0,90,GREEN,BLUE,"3.5\" ILI9488 320X480",32,1);//������ʾ
//	Gui_StrCenter(0,120,GREEN,BLUE,"3.5\" ILI9488 320X480",32,1);
	//Gui_StrCenter(0,120,BLUE,BLUE,"xiaoFeng@QDtech 2018-08-06",32,1);//������ʾ
	printf("maintest\n");
	 vTaskDelay(1500/portTICK_RATE_MS); 
	  vTaskDelay(1500/portTICK_RATE_MS); 
	//delay_ms(1500);		
	//delay_ms(1500);
}


/**
 * 电量显示：长40为满格电量
 */
void Battle_Power(uint8_t power){
	int i,j;
	for(i=0;i<power*40*0.01;i++){
		for(j=0;j<18;j++){
			GUI_DrawPoint(i+170,j+12,WHITE);
		}
	}
	IntToStr_8(str_power,power);
	Show_Str(178,13,BLUE,BLUE,str_power,32,1);
}

/**
 * 将uint8_t转换成字符串
 */
void IntToStr_8(char *str ,uint8_t data){
	if(data >= 100)
	{
		str[0] = data/100 + 48;
		str[1] = (data/10)%10 + 48;
		str[2] = data%10 + 48;
		str[3] = 0;
	}
	else if(data >= 10)
	{
		str[0] = data/10 + 48;
		str[1] = data%10 + 48;
		str[2] = 0;
	}
	else
	{
		str[0] = data + 48;
		str[1] = 0;
	}
}

/**
 * 将uint16_t转换成字符串
 */
void IntToStr_16(char *str ,uint16_t data){
	if(data >= 10000)
	{
		str[0] = data/10000 + 48;
		str[1] = (data/1000)%10 + 48;
		str[2] = (data/100)%10 + 48;
		str[3] = (data/10)%10 + 48;
		str[4] = data%10 + 48;
		str[5] = 0;
	}
	else if(data >= 1000)
	{
		str[0] = data/1000 + 48;
		str[1] = (data/100)%10 + 48;
		str[2] = (data/10)%10 + 48;
		str[3] = data%10 + 48;
		str[4] = 0;
	}
	else if(data >= 100)
	{
		str[0] = data/100 + 48;
		str[1] = (data/10)%10 + 48;
		str[2] = data%10 + 48;
		str[3] = 0;
	}
	else if(data >= 10)
	{
		str[0] = data/10 + 48;
		str[1] = data%10 + 48;
		str[2] = 0;
	}
	else
	{
		str[0] = data + 48;
		str[1] = 0;
	}
}


/**
 * 将字符串转换成uint16_t
 */
uint16_t StrToInt_16(char *str)
{
	uint16_t i,Value=0;
	uint16_t tmp;
	for(i = strlen(str);i > 0;i --)
	{
		if(*str >= '0' && *str <= '9'){
			tmp = pow(10,i-1);
			Value += (*str-48)*tmp;
		}
		else
			break;
		str++;
	}
	return Value;
}

/**
 * 将字符串转换成uint8_t
 */
uint8_t StrToInt_8(char *str)
{
	uint8_t i,Value=0;
	uint8_t tmp;
	for(i = strlen(str);i > 0;i --)
	{
		if(*str >= '0' && *str <= '9'){
			tmp = pow(10,i-1);
			Value += (*str-48)*tmp;
		}
		else
			break;
		str++;
	}
	return Value;
}

/**
 *将电阻值转换为字符串（有小数点） 
 */
void ImpToStr(char *str, uint16_t imp)
{
	if(imp/1000 >= 10)
	{
		str[0] = imp/10000 + 48;
		str[1] = (imp/1000)%10 + 48;
		str[2] = 0x2E;
		str[3] = (imp/100)%10 + 48;
		str[4] = (imp/10)%10 +48;
		str[5] = 0;
	}
	else if(imp/1000 >= 1)
	{
		str[0] = imp/1000 + 48;
		str[1] = 0x2E;
		str[2] = (imp/100)%10 + 48;
		str[3] = (imp/10)%10 +48;
		str[4] = imp%10 + 48;
		str[5] = 0;
	}
	else if(imp/1000 >0.1)
	{
		str[0] = 0x30;
		str[1] = 0x2E;
		str[2] = imp/100 + 48;
		str[3] = (imp/10)%10 +48;
		str[4] = imp%10 + 48;
		str[5] = 0;
	}
	else
	{
		//小于0.1千欧都为0
		str[0] = 0x30;
		str[1] = 0;
	}
}



/**
 *显示模式及相关参数 
 */
void Show_Data(char *a, char *b, char *c, char *d, char *e, char *f, char *g){
	//显示模式
	Show_Str(120,45,WHITE,BLUE,a,48,1);
	//显示电流
	Show_Str(120,105,WHITE,BLUE,b,48,1);
	//显示脉宽
	Show_Str(120,170,WHITE,BLUE,c,48,1);
	//显示间歇
	Show_Str(120,230,WHITE,BLUE,d,48,1);
	//显示时间
	Show_Str(120,295,WHITE,BLUE,e,48,1);
	//显示阻抗
	Show_Str(28,370,WHITE,BLUE,f,48,1);
	//显示电流
	Show_Str(180,370,WHITE,BLUE,g,48,1);
}

void LCD_task(void* arg)
{
     LCD_Init();   
   
    while(1)
    {
    LCD_main_test();
        vTaskDelay(10/portTICK_RATE_MS);

    }
}

/*****************************************************************************
 * @name       :void Test_Color(void)
 * @date       :2018-08-09 
 * @function   :Color fill test(white,black,red,green,blue)
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*void Test_Color(void)
{
	//DrawTestPage("����1:��ɫ������");
	LCD_Fill(0,0,lcddev.width,lcddev.height,WHITE);
	Show_Str(20,30,BLUE,YELLOW,"BL Test",16,1);//delay_ms(800);
	LCD_Fill(0,0,lcddev.width,lcddev.height,RED);
	Show_Str(20,30,BLUE,YELLOW,"RED ",16,1);//delay_ms(800);
	LCD_Fill(0,0,lcddev.width,lcddev.height,GREEN);
	Show_Str(20,30,BLUE,YELLOW,"GREEN ",16,1);//delay_ms(800);
	LCD_Fill(0,0,lcddev.width,lcddev.height,BLUE);
	Show_Str(20,30,RED,YELLOW,"BLUE ",16,1);//delay_ms(800);
}
*/
/*****************************************************************************
 * @name       :void Test_FillRec(void)
 * @date       :2018-08-09 
 * @function   :Rectangular display and fill test
								Display red,green,blue,yellow,pink rectangular boxes in turn,
								1500 milliseconds later,
								Fill the rectangle in red,green,blue,yellow and pink in turn
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*void Test_FillRec(void)
{
	uint8_t i=0;
	DrawTestPage("����2:GUI����������");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for (i=0; i<5; i++) 
	{
		POINT_COLOR=ColorTab[i];
		LCD_DrawRectangle(lcddev.width/2-80+(i*15),lcddev.height/2-80+(i*15),lcddev.width/2-80+(i*15)+60,lcddev.height/2-80+(i*15)+60); 
	}
	//delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for (i=0; i<5; i++) 
	{
		POINT_COLOR=ColorTab[i];
		LCD_DrawFillRectangle(lcddev.width/2-80+(i*15),lcddev.height/2-80+(i*15),lcddev.width/2-80+(i*15)+60,lcddev.height/2-80+(i*15)+60); 
	}
	//delay_ms(1500);
}
*/
/*****************************************************************************
 * @name       :void Test_Circle(void)
 * @date       :2018-08-09 
 * @function   :circular display and fill test
								Display red,green,blue,yellow,pink circular boxes in turn,
								1500 milliseconds later,
								Fill the circular in red,green,blue,yellow and pink in turn
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*void Test_Circle(void)
{
	uint8_t i=0;
	DrawTestPage("����3:GUI��Բ������");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for (i=0; i<5; i++)  
		gui_circle(lcddev.width/2-80+(i*25),lcddev.height/2-50+(i*25),ColorTab[i],30,0);
	//delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for (i=0; i<5; i++) 
	  	gui_circle(lcddev.width/2-80+(i*25),lcddev.height/2-50+(i*25),ColorTab[i],30,1);
	//delay_ms(1500);
}
*/
/*****************************************************************************
 * @name       :void English_Font_test(void)
 * @date       :2018-08-09 
 * @function   :English display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*void English_Font_test(void)
{
	DrawTestPage("����5:Ӣ����ʾ����");
	Show_Str(10,30,BLUE,YELLOW,"6X12:abcdefghijklmnopqrstuvwxyz0123456789",12,0);
	Show_Str(10,45,BLUE,YELLOW,"6X12:ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",12,1);
	Show_Str(10,60,BLUE,YELLOW,"6X12:~!@#$%^&*()_+{}:<>?/|-+.",12,0);
	Show_Str(10,80,BLUE,YELLOW,"8X16:abcdefghijklmnopqrstuvwxyz0123456789",16,0);
	Show_Str(10,100,BLUE,YELLOW,"8X16:ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789",16,1);
	Show_Str(10,120,BLUE,YELLOW,"8X16:~!@#$%^&*()_+{}:<>?/|-+.",16,0); 
	//delay_ms(1200);
}
*/
/*****************************************************************************
 * @name       :void Test_Triangle(void)
 * @date       :2018-08-09 
 * @function   :triangle display and fill test
								Display red,green,blue,yellow,pink triangle boxes in turn,
								1500 milliseconds later,
								Fill the triangle in red,green,blue,yellow and pink in turn
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*void Test_Triangle(void)
{
	uint8_t i=0;
	DrawTestPage("����4:GUI Triangle������");
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
	for(i=0;i<5;i++)
	{
		POINT_COLOR=ColorTab[i];
		Draw_Triangel(lcddev.width/2-80+(i*20),lcddev.height/2-20+(i*15),lcddev.width/2-50-1+(i*20),lcddev.height/2-20-52-1+(i*15),lcddev.width/2-20-1+(i*20),lcddev.height/2-20+(i*15));
	}
//	delay_ms(1500);	
	LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE); 
	for(i=0;i<5;i++)
	{
		POINT_COLOR=ColorTab[i];
		Fill_Triangel(lcddev.width/2-80+(i*20),lcddev.height/2-20+(i*15),lcddev.width/2-50-1+(i*20),lcddev.height/2-20-52-1+(i*15),lcddev.width/2-20-1+(i*20),lcddev.height/2-20+(i*15));
	}
	//delay_ms(1500);
}
*/
/*****************************************************************************
 * @name       :void Chinese_Font_test(void)
 * @date       :2018-08-09 
 * @function   :chinese display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*void Chinese_Font_test(void)
{	
	DrawTestPage("����6:������ʾ����");
	Show_Str(10,30,BLUE,YELLOW,"16X16:ȫ�����Ӽ������޹�˾��ӭ��",16,0);
	Show_Str(10,50,BLUE,YELLOW,"16X16:Welcomeȫ������",16,0);
	Show_Str(10,70,BLUE,YELLOW,"24X24:���������Ĳ���",24,1);
	Show_Str(10,100,BLUE,YELLOW,"32X32:�������",32,1);
	//delay_ms(1200);
}
*/
/*****************************************************************************
 * @name       :void Pic_test(void)
 * @date       :2018-08-09 
 * @function   :picture display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*void Pic_test(void)
{
	DrawTestPage("����7:ͼƬ��ʾ����");
	//LCD_Fill(0,20,lcddev.width,lcddev.height-20,WHITE);
//	Gui_Drawbmp16(30,30,gImage_qq);
	Show_Str(30+12,75,BLUE,YELLOW,"QQ",16,1);
//	Gui_Drawbmp16(90,30,gImage_qq);
	Show_Str(90+12,75,BLUE,YELLOW,"QQ",16,1);
//	Gui_Drawbmp16(150,30,gImage_qq);
	Show_Str(150+12,75,BLUE,YELLOW,"QQ",16,1);
	//delay_ms(1200);
}
*/
/*****************************************************************************
 * @name       :void Rotate_Test(void)
 * @date       :2018-08-09 
 * @function   :rotate test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
/*void Rotate_Test(void)
{
	uint8_t i=0;
	uint8_t *Direction[4]={"Rotation:0","Rotation:90","Rotation:180","Rotation:270"};
	
	for(i=0;i<4;i++)
	{
	LCD_direction(i);
	DrawTestPage("����8:��Ļ��ת����");
	Show_Str(20,30,BLUE,YELLOW,Direction[i],16,1);
//	Gui_Drawbmp16(30,50,gImage_qq);
	//delay_ms(1000);delay_ms(1000);
	}
	LCD_direction(USE_HORIZONTAL);
}*/





