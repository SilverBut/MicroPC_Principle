#include "serial_debugger.h"

#define blink(group, pin)   do{if (GPIO_ReadOutputDataBit((group), (pin))) \
							GPIO_ResetBits((group), (pin)); \
							else GPIO_SetBits((group), (pin));}\
							while(0);

void serialdbg_printstr(char* str){
	while(*str!='\0'){
		if (*str == '\n')
			USART1_SENDOUT('\r');
		USART1_SENDOUT(*str++);
	}
}


void USART1_IRQHandler(void){
	unsigned char x;
	if (USART_GetITStatus(USART1, USART_IT_RXNE)==SET){
		/* Received a byte */
		#ifdef SEPERATE_LED_USART
		blink(LEDGroup, LEDRxPort);
		#endif
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

void UsartInitalize(void){

	USART_InitTypeDef usart_initval;
	GPIO_InitTypeDef gpio_initval;
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
	
	/* Clock Initalize*/
	RCC_APB2PeriphClockCmd(UsartClock, ENABLE);		
	/* Configurements of USART1 */
	usart_initval.USART_BaudRate = 9600;									//baudrate is 600. notice sysclock
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

void serialdbg_init(void){
	UsartInitalize();
}
