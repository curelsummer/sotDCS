#include "button.h"
#include "timer.h"

KyeStrTypeDef KEYS[KEYn];
__IO uint8_t KeyUpdateFlag = 0; 

const uint64_t KEY_PIN_SEL[KEYn] = {KEY_DEC_SEL,
                                                  KEY_INC_SEL,                               
							 KEY_SEL_SEL,
							 KEY_ENTER_SEL,
							};

const uint16_t KEY_PIN[KEYn] = {KEY_DEC_PIN,
                                                  KEY_INC_PIN,                               
							 KEY_SEL_PIN,
							 KEY_ENTER_PIN,
							};




void BSP_KEY_Init(KeyOrderEnum key)
{
	gpio_config_t io_conf;	

    io_conf.pin_bit_mask = KEY_PIN_SEL[key];
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

	KEYS[key].PushCounter = 0;//按下时间计数器
	KEYS[key].CycleClcCounter = 0;//循环响应周期计数器
	KEYS[key].KeyStatus = (KeyStatusDef)gpio_get_level(KEY_PIN[key]);
	KEYS[key].KeyEvent_push = STATUS_IDLE;
	KEYS[key].KeyEvent_pop = STATUS_IDLE;
	KEYS[key].KeyEvent_long_click = STATUS_IDLE;
	KEYS[key].KeyEvent_long_cycle_click = STATUS_IDLE;
}

KeyStatusDef BSP_KEY_GetState(KeyOrderEnum key)
{
    return (KeyStatusDef)gpio_get_level(KEY_PIN[key]);
}  

void Key_Init(void)
{
	BSP_KEY_Init(KEY_DEC);
	BSP_KEY_Init(KEY_INC);	
	BSP_KEY_Init(KEY_SEL);
	BSP_KEY_Init(KEY_ENTER);
	timers_init();
	
}

void Key_StaUpdate(void)
{
	int i;
	
	for(i = 0 ; i < KEYn ; i++)
	{
		if(KEY_STATUS_DOWN == BSP_KEY_GetState((KeyOrderEnum)i))
		{
            
			if(KEYS[i].KeyStatus == KEY_STATUS_UP)//update key status, set KeyEvent_push
			{
				KEYS[i].KeyStatus = KEY_STATUS_DOWN;
				
				if(KEYS[i].KeyEvent_push == STATUS_IDLE)
				{			
					KEYS[i].KeyEvent_pop = STATUS_IDLE;
					KEYS[i].KeyEvent_push = STATUS_READY;
				}
			}
			
			if(KEYS[i].PushCounter < LONG_CLC_TIME)//set KeyEvent_long_click
			{
				KEYS[i].PushCounter++;
			}
			else if(KEYS[i].KeyEvent_long_click == STATUS_IDLE)
			{
				KEYS[i].KeyEvent_long_click = STATUS_READY;
			}
			
			if(KEYS[i].KeyEvent_long_click != STATUS_IDLE)//set KeyEvent_long_cycle_click
			{
				if(KEYS[i].CycleClcCounter < LONG_CLC_CYC)
				{
					KEYS[i].CycleClcCounter++;
				}
				else
				{
					KEYS[i].KeyEvent_long_cycle_click = STATUS_READY;
				}
			}
		}
		else
		{
			if(KEYS[i].KeyStatus == KEY_STATUS_DOWN)//update key status, set KeyEvent_pop
			{
				KEYS[i].KeyStatus = KEY_STATUS_UP;
				KEYS[i].KeyEvent_push = STATUS_IDLE;
				KEYS[i].KeyEvent_pop = STATUS_READY;
				
			}
			
			KEYS[i].PushCounter = 0;//清按下时间计数器
			KEYS[i].CycleClcCounter = 0;//清循环响应周期计数器
			
			KEYS[i].KeyEvent_push = STATUS_IDLE;
			KEYS[i].KeyEvent_long_click = STATUS_IDLE;
			KEYS[i].KeyEvent_long_cycle_click = STATUS_IDLE;
		}
	}	
}

void Key_EventClr(void)
{
	uint8_t i;
	for(i=0;i<KEYn;i++)
	{
		KEYS[i].PushCounter = 0;//按下时间计数器
		KEYS[i].CycleClcCounter = 0;//循环响应周期计数器
		
		KEYS[i].KeyEvent_push = STATUS_IDLE;
		KEYS[i].KeyEvent_pop = STATUS_IDLE;
		KEYS[i].KeyEvent_long_click = STATUS_IDLE;
		KEYS[i].KeyEvent_long_cycle_click = STATUS_IDLE;
	}
}

void Key_EventClrSingle(KeyOrderEnum key)
{

	KEYS[key].PushCounter = 0;//按下时间计数器
	KEYS[key].CycleClcCounter = 0;//循环响应周期计数器
	
	KEYS[key].KeyEvent_push = STATUS_IDLE;
	KEYS[key].KeyEvent_pop = STATUS_IDLE;
	KEYS[key].KeyEvent_long_click = STATUS_IDLE;
	KEYS[key].KeyEvent_long_cycle_click = STATUS_IDLE;
}


uint8_t Key_GetEvent(KeyOrderEnum key,KeyEventTypeDef Event)
{
	switch(Event)
	{
		case KEY_TYPE_SHORT_CLC:
			if(KEYS[key].KeyEvent_push == STATUS_READY)
			{
				KEYS[key].KeyEvent_push = STATUS_PENDING;
				return 1;
			}
			else
			{ 
				return 0;
			} 
//			break;

		case KEY_TYPE_LONG_CLC_ONCE:
			if(KEYS[key].KeyEvent_long_click == STATUS_READY)
			{
				 KEYS[key].KeyEvent_long_click = STATUS_PENDING;
				 return 1;
			}
			else
			{
				return 0;
			}
//			break;
		case KEY_TYPE_LONG_CLC_CYC:
			if(KEYS[key].KeyEvent_long_cycle_click == STATUS_READY)
			{
				KEYS[key].KeyEvent_long_cycle_click = STATUS_IDLE;
				KEYS[key].CycleClcCounter = 0;
				
				return 1; 
			}
			else 
			{
				return 0;
			}
//			break;
		case KEY_TYPE_RISE:
			if(KEYS[key].KeyEvent_pop == STATUS_READY)
			{
				KEYS[key].KeyEvent_pop = STATUS_PENDING;
				return 1;
			}
			else
			{
				return 0;
			}
		case KEY_TYPE_STATUS:
			if(KEYS[key].KeyStatus == KEY_STATUS_UP)
			{
				return 1;
			}
			else
			{
				return 0;
			}
			
//			break;

		default: return 0;
	}
}




