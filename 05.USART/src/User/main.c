#include "stm32f10x.h"

#define blink(group, pin)   do{if (GPIO_ReadOutputDataBit((group), (pin))) \
							GPIO_ResetBits((group), (pin)); \
							else GPIO_SetBits((group), (pin));}\
							while(0);
	
void GpioInitalize(void);
void UsartInitalize(void);
void TimerInitalize(void);

#define UsartTxPort GPIO_Pin_9
#define UsartRxPort GPIO_Pin_10
#define UsartGroup GPIOA
#define UsartGPIOClock RCC_APB2Periph_GPIOA
#define UsartClock RCC_APB2Periph_USART1

#define LEDTxPort GPIO_Pin_7
#define LEDRxPort GPIO_Pin_8
#define LEDRunPort GPIO_Pin_9
#define LEDGroup GPIOE
#define LEDGPIOClock RCC_APB2Periph_GPIOE

#define USART1_SENDOUT(data) do{USART_SendData(USART1,data);\
					blink(LEDGroup, LEDTxPort);\
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);}while(0);
#define USART1_SENDCRLF() do{					USART_SendData(USART1,'\r');\
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);\
					USART_SendData(USART1,'\n');}while(0);
#define ISDBG
#undef ISDBG

void printstr(char* str);

int main(void){
	//Initalize On-chip Periphals
	GpioInitalize();
	UsartInitalize();
	TimerInitalize();
	
	printstr("\n\n");
	printstr("This is a serial commander!\n");
	printstr("Type and I'll tell you what you've typed!\n");
	while(1);
}


void GpioInitalize(void){

	GPIO_InitTypeDef gpio_initval;

	/* Open clock */
	RCC_APB2PeriphClockCmd(LEDGPIOClock, ENABLE);
	RCC_APB2PeriphClockCmd(UsartGPIOClock, ENABLE);

	gpio_initval.GPIO_Speed = GPIO_Speed_50MHz;
	/* Tx port set*/
	gpio_initval.GPIO_Pin = UsartTxPort;
	gpio_initval.GPIO_Mode = GPIO_Mode_AF_PP;	//Need stronger output
	GPIO_Init(UsartGroup, &gpio_initval);
	/* Rx port set*/
	gpio_initval.GPIO_Pin = UsartRxPort;
	gpio_initval.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//Need weeker input
	GPIO_Init(UsartGroup, &gpio_initval);

	/* LEDs Set*/
	gpio_initval.GPIO_Pin = LEDTxPort;	//Tx led
	gpio_initval.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDGroup, &gpio_initval);
	gpio_initval.GPIO_Pin = LEDRxPort;	//Rx led
	gpio_initval.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDGroup, &gpio_initval);
	gpio_initval.GPIO_Pin = LEDRunPort;	//heartbeat led
	gpio_initval.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LEDGroup, &gpio_initval);

}


void UsartInitalize(void){

	USART_InitTypeDef usart_initval;
	
	/* Clock Initalize*/
	RCC_APB2PeriphClockCmd(UsartClock, ENABLE);		
	/* Configurements of USART1 */
	usart_initval.USART_BaudRate = 600;									//baudrate is 600. notice sysclock
	usart_initval.USART_HardwareFlowControl 
					= USART_HardwareFlowControl_None ;						//no flow control
	usart_initval.USART_StopBits = USART_StopBits_1;			//1s
	usart_initval.USART_WordLength = USART_WordLength_8b;	//8d
	usart_initval.USART_Parity = USART_Parity_No;					//No check
	usart_initval.USART_Mode = 
						USART_Mode_Rx | USART_Mode_Tx;							//Do both job
	/* Write configure */
	USART_Init(USART1, &usart_initval);
	/* USART1 IRQn Triggerable */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* USART1 Bootup */
	USART_Cmd(USART1, ENABLE);
	/* USART1 IRQn NVIC Interruptable */
	NVIC_EnableIRQ(USART1_IRQn);
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
	tim_tb_initval.TIM_Period =  500-1;	  	
	TIM_TimeBaseInit(TIM2, &tim_tb_initval);	//apply settings
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);		//clear flag
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);	//can set interruption
	TIM_Cmd(TIM2, ENABLE);		//enable Timer2
	NVIC_EnableIRQ(TIM2_IRQn);	//Enable IRQ in NVIC

}

void USART1_IRQHandler(void){
	unsigned char x;
	if (USART_GetITStatus(USART1, USART_IT_RXNE)==SET){
		/* Received a byte */
		blink(LEDGroup, LEDRxPort);
		x=USART_ReceiveData(USART1);	//RXNE will be hw-cls by reading data
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	//wait till idle
		if (USART_GetITStatus(USART1, USART_IT_TXE)==RESET)	//make sure sendable
		switch (x) {
				case '\r'://make special process for CRLF
					USART1_SENDCRLF();
					break;
				case 0x7F:
				USART1_SENDOUT(8);
				USART1_SENDOUT(' ');
				USART1_SENDOUT(8);
					break;
				default:
				#ifdef ISDBG
					USART1_SENDOUT('*');
					USART1_SENDOUT(x);
					USART1_SENDOUT('=');
					USART_SendData(USART1, x);//send data back
					x=(x>>4)|(x<<4);
					USART1_SENDOUT((x&0xf)>10?(x&0xf)-10+'A':(x&0xf)+'0');//send data back
					x>>=4;
					USART1_SENDOUT((x&0xf)>10?(x&0xf)-10+'A':(x&0xf)+'0');//send data back
					USART1_SENDOUT('*');
				#else
					USART1_SENDOUT(x);
				#endif
		}
	}
}

void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update)!=RESET){
		TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);	//Clear IT Flag
		blink(LEDGroup, LEDRunPort);	//Show the heartbeat
	}
}

void printstr(char* str){
	while(*str!='\0'){
		if (*str == '\n')
			USART1_SENDOUT('\r');
		USART1_SENDOUT(*str++);
	}
}