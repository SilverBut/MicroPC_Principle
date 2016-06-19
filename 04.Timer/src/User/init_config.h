#ifndef __INIT_CONFIG_H__
#define __INIT_CONFIG_H__

#include "stm32f10x.h"

#define LED_numbers 4
//config output (pp-out)
GPIOMode_TypeDef LED_MODE[LED_numbers] = {GPIO_Mode_Out_PP,GPIO_Mode_Out_PP,
										GPIO_Mode_Out_PP,GPIO_Mode_Out_PP};
//clock speed 50m
GPIOSpeed_TypeDef LED_SPEED[LED_numbers] = {GPIO_Speed_50MHz,GPIO_Speed_50MHz,
													GPIO_Speed_50MHz,GPIO_Speed_50MHz};
//ports e7~e10
uint16_t LED_PIN[LED_numbers] = {GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10};
GPIO_TypeDef* LED_GROUP[LED_numbers] = {GPIOE,GPIOE,GPIOE,GPIOE};





#endif
