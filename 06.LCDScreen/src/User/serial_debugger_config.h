#ifndef __SERIALDBG_CFG_H__
#define __SERIALDBG_CFG_H__

#define UsartTxPort GPIO_Pin_9
#define UsartRxPort GPIO_Pin_10
#define UsartGroup GPIOA
#define UsartGPIOClock RCC_APB2Periph_GPIOA
#define UsartClock RCC_APB2Periph_USART1

#ifdef SEPERATE_LED_USART
#define LEDTxPort GPIO_Pin_7
#define LEDRxPort GPIO_Pin_8
#define LEDGroup GPIOB
#define LEDGPIOClock RCC_APB2Periph_GPIOB
#endif



#endif
