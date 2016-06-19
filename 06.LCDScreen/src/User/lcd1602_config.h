#ifndef __LCD1602_CFG_H__
#define __LCD1602_CFG_H__

#define LCD1602_RS 		 GPIO_Pin_13
#define LCD1602_RW 		 GPIO_Pin_14
#define LCD1602_E  		 GPIO_Pin_15

#define LCD1602_RS_GROUP GPIOC
#define LCD1602_RW_GROUP GPIOC
#define LCD1602_E_GROUP  GPIOC

#define LCD1602_RS_CLOCK RCC_APB2Periph_GPIOC
#define LCD1602_RW_CLOCK RCC_APB2Periph_GPIOC
#define LCD1602_E_CLOCK  RCC_APB2Periph_GPIOC

#define LCD1602_CLOCK_Cmd(X,Y) RCC_APB2PeriphClockCmd((X),(Y))

//We need to use macro to deal with different type of value read/write to increase speed
//In this example our data is from E7->E14
//which means to read it, just read group E and have E>>7
#define LCD1602_WRITE_DATABAND(data) do{ GPIOF->BSRR = ((data&0xff) | ((~data)&0xff)<<(+16)) ;}while(0);
#define LCD1602_READ_DATABAND 		(((GPIOF->IDR))&0xff)

//Follow options define initalize behaviour
#define LCD1602_CAN_SCREENMOVE
#define LCD1602_INIT_CLS
#define LCD1602_LINE_COUNT 2
#define LCD1602_CHAR_COUNT 16
#define LCD1602_INIT_OPTIONS ( LCD1602_CURSOR_AUTORIGHT | LCD1602_DISP_ON | LCD1602_CURSOR_OFF | LCD1602_CURSOR_STILL )

#endif
