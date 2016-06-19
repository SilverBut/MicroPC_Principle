#ifndef __SERIALDBG_DRV_H__
#define __SERIALDBG_DRV_H__

#include "stm32f10x.h"
#include "serial_debugger_config.h"

#ifdef SEPERATE_LED_USART
#define USART1_SENDOUT(data) do{USART_SendData(USART1,data);\
					blink(LEDGroup, LEDTxPort);\
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);}while(0);
#else
#define USART1_SENDOUT(data) do{USART_SendData(USART1,data);\
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);}while(0);
#endif

#define USART1_SENDCRLF() do{					USART_SendData(USART1,'\r');\
					while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);\
					USART_SendData(USART1,'\n');}while(0);



void serialdbg_printstr(char* str);
void serialdbg_init(void);

#endif
