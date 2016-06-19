#include "heartrun_mon.h"
#include "stm32f10x.h"


#define blink(group, pin)   do{if (GPIO_ReadOutputDataBit((group), (pin))) \
							GPIO_ResetBits((group), (pin)); \
							else GPIO_SetBits((group), (pin));}\
							while(0);

void GpioInitalize(void){
	GPIO_InitTypeDef gpio_initval;

	/* Open clock */
	RCC_APB2PeriphClockCmd(LEDGPIOClock, ENABLE);
	gpio_initval.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_initval.GPIO_Pin = LEDRunPort;	//heartbeat led
	gpio_initval.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDGroup, &gpio_initval);
}

void TimerInitalize(void){

	TIM_TimeBaseInitTypeDef tim_tb_initval;

	//Enable timer of heart beat
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  
	TIM_DeInit(TIM2);	 //Reset to default
	/* Set timer to 4Hz */
	tim_tb_initval.TIM_Prescaler = 36000; 
	tim_tb_initval.TIM_ClockDivision = TIM_CKD_DIV1;	
	tim_tb_initval.TIM_CounterMode = TIM_CounterMode_Up;	
	tim_tb_initval.TIM_Period =  300-1;//0.15s	  	
	TIM_TimeBaseInit(TIM2, &tim_tb_initval);	//apply settings
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);		//clear flag
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	//can set interruption
	TIM_Cmd(TIM2, ENABLE);		//enable Timer2
	NVIC_EnableIRQ(TIM2_IRQn);	//Enable IRQ in NVIC

}

void heartrun_mon_init(void){
	GpioInitalize();
	TimerInitalize();
}

void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);	//Clear IT Flag
		blink(LEDGroup, LEDRunPort);	//Show the heartbeat
	}
}
