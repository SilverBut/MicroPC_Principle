#include "stm32f10x.h"
#include "init_config.h"

// Initilization code
void DeviceInit(void);

#define blink(group, pin)   do{if (GPIO_ReadOutputDataBit((group), (pin)))	GPIO_ResetBits((group), (pin)); else	GPIO_SetBits((group), (pin));}while(0);

int main(void){
	
	DeviceInit();
	
	while(1);
	return 0;
}

void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);
		blink(LED_GROUP[0], LED_PIN[0]);
	}
}

void TIM1_UP_TIM16_IRQHandler(void){
	if (TIM_GetITStatus(TIM1, TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM1, TIM_FLAG_Update);
		blink(LED_GROUP[1], LED_PIN[1]);
	}
}

void SysTick_Handler(void){
	static int systick = 0 ;
	if (systick==250){//500ms
		blink(LED_GROUP[2], LED_PIN[2]);
		systick=0;
	}else
	systick++;
}

void TIM7_IRQHandler(void){
	if (TIM_GetITStatus(TIM7, TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM7, TIM_FLAG_Update);
		blink(LED_GROUP[3], LED_PIN[3]);
	}
}


void DeviceInit(void){
	int i = 0;
	GPIO_InitTypeDef gpio_initval;
	NVIC_InitTypeDef nvic_initval;
	TIM_TimeBaseInitTypeDef tim_tb_initval;
	
	
	
	// System clock init
	//SystemInit();
	
	/*
	 * GPIO Initalization
	 *
	 * Since we're using LEDs as a sign of timer's action,
	 * we need to configure some IO ports. LEDs' negative
	 * should be connected to ports defined in init_config.h, 
	 * and 3.3V is connected via a 1K res to positive.
	*/

	//Connect GPIO clock to device clock bus
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	for ( i = 0 ; i < LED_numbers; i++ ){
		//Config ports
		gpio_initval.GPIO_Pin = LED_PIN[i];
		gpio_initval.GPIO_Mode = LED_MODE[i];
		gpio_initval.GPIO_Speed = LED_SPEED[i];
		GPIO_Init(LED_GROUP[i], &gpio_initval);
		//Comment following line if you want to turn on LED
		GPIO_SetBits(LED_GROUP[i], LED_PIN[i]);
	}
	
	/*
	 * Timer Initalization
	 *
	 * 
	 */
	//configure TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //Connect TIM2 clock to device clock bus
	TIM_DeInit(TIM2);	 //reset to default configure
	//now tim2 clock is 2*(APB1->PCLK1->HCLK/2->SYSCLK/2->36Mhz) = 72Mhz
	tim_tb_initval.TIM_Prescaler = 36000; //72Mhz / 36000 = 2kHz
	tim_tb_initval.TIM_ClockDivision = TIM_CKD_DIV1;	//enable scaler
	tim_tb_initval.TIM_CounterMode = TIM_CounterMode_Up;	//count up
	tim_tb_initval.TIM_Period =  1000;	  	//2khz/1000 = 0.5s
	TIM_TimeBaseInit(TIM2, &tim_tb_initval);	//apply settings
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);		//clear flag
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	//enable interruption
	TIM_Cmd(TIM2, ENABLE);	//enable Timer2
	
	//configure TIM0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  //Connect TIM1 clock to device clock bus
	TIM_DeInit(TIM1);	//config reset
	tim_tb_initval.TIM_Period = 36000;	//2khz
	tim_tb_initval.TIM_Prescaler = 1000;	//0.5s
	tim_tb_initval.TIM_RepetitionCounter = 1;	//0.5*(0+1)=1s
	TIM_TimeBaseInit(TIM1, &tim_tb_initval);
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM1, ENABLE);	//enable Timer1
	
	//configure TIM7
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);  //Connect TIM7 clock to device clock bus
	TIM_DeInit(TIM7);	//config reset
	tim_tb_initval.TIM_Period = 36000;	//2khz
	tim_tb_initval.TIM_Prescaler = 250;	//0.25s
	TIM_TimeBaseInit(TIM7, &tim_tb_initval);
	TIM_ClearFlag(TIM7, TIM_FLAG_Update);
	TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM7, ENABLE);	//enable Timer1
	
	/*
	 * NVIC Initalization
	 *
	 * Since timers will trigger some interruptions, we need
	 * to set up the NVICs now. Priorities and some other
	 * values are configured here.
	 */
	//configure TIM2 (General-purpose timers) NVIC
	nvic_initval.NVIC_IRQChannel = TIM2_IRQn;
	nvic_initval.NVIC_IRQChannelCmd = ENABLE;
	nvic_initval.NVIC_IRQChannelSubPriority = 1;
	nvic_initval.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&nvic_initval);
	
	//configure TIM0 (Advanced timer) NVIC
	nvic_initval.NVIC_IRQChannel = TIM1_UP_IRQn;
	nvic_initval.NVIC_IRQChannelCmd = ENABLE;
	nvic_initval.NVIC_IRQChannelSubPriority = 2;
	nvic_initval.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&nvic_initval);
	
	//configure SYSTick interruption
	SysTick_Config(SystemCoreClock/1000);
	
	//configure TIM7 (Basic timer) NVIC
	nvic_initval.NVIC_IRQChannel = TIM7_IRQn;
	nvic_initval.NVIC_IRQChannelCmd = ENABLE;
	nvic_initval.NVIC_IRQChannelSubPriority = 2;
	nvic_initval.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Init(&nvic_initval);
	return;
}
