#ifndef _BHUTTON_H
#define _BHUTTON_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "../includes_common/includes_common.h"

#define     __IO    volatile 

#define KEYn							4

#define KEY_DEC_PIN						 (36)

#define KEY_INC_PIN					 (37)

#define KEY_SEL_PIN					 (38)

#define KEY_ENTER_PIN					 (39)

#define KEY_DEC_SEL				(1ULL<<KEY_DEC_PIN) //GPIO_SEL_36	//	 (36)

#define KEY_INC_SEL		       (1ULL<<KEY_INC_PIN) //GPIO_SEL_37	//	 (37)

#define KEY_SEL_SEL			(1ULL<<KEY_SEL_PIN) 	//	 (38)

#define KEY_ENTER_SEL			(1ULL<<KEY_ENTER_PIN) 	//	 (39)



//定义短按和长按临界时间。单位50ms 如:LONG_CLC=30时,按键按下时间大于30*50ms=1.5s时系统判定为长按
#define LONG_CLC_TIME	20

//定义长按时触发事件的周期。单位50ms 如LONG_FREQ=10时，系统每2*50=100ms响应一次按键，即每秒钟响应10次长按事件
#define LONG_CLC_CYC	2

typedef enum
{
	KEY_DEC = 0,
	KEY_INC = 1,
	KEY_SEL = 2,
	KEY_ENTER = 3,
}KeyOrderEnum;

typedef enum
{
	KEY_STATUS_DOWN = 0,
	KEY_STATUS_UP = 1
}KeyStatusDef;

typedef enum
{
	STATUS_IDLE = 0,//表示事件未发生
	STATUS_READY,//表示事件已经发生，未被响应
	STATUS_PENDING,//表示事件发生，并已经响应。
	STATUS_RESET//表示按键还未初始化
}EventStatusDef;

typedef struct
{
	// KeyStatusDef StatusValid;//按下后是高电平还是低电平
	__IO EventStatusDef KeyInit;
	__IO uint8_t PushCounter;//按下时间计数器
	__IO uint8_t CycleClcCounter;//循环响应周期计数器
	
	__IO KeyStatusDef KeyStatus;
	
	__IO EventStatusDef KeyEvent_push;
	__IO EventStatusDef KeyEvent_pop;
	__IO EventStatusDef KeyEvent_long_click;
	__IO EventStatusDef KeyEvent_long_cycle_click;
}KyeStrTypeDef;

typedef enum
{
	KEY_TYPE_SHORT_CLC,//按键按下
	KEY_TYPE_LONG_CLC_ONCE,
	KEY_TYPE_LONG_CLC_CYC,
	KEY_TYPE_RISE,//按键抬起
	KEY_TYPE_STATUS
}KeyEventTypeDef;



void Key_Init(void);
void Key_StaUpdate(void);
uint8_t Key_GetEvent(KeyOrderEnum key,KeyEventTypeDef Event);
void Key_EventClr(void);
void Key_EventClrSingle(KeyOrderEnum key);


#ifdef __cplusplus
}
#endif

#endif


