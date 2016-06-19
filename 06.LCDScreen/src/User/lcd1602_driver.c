#include "lcd1602_driver.h"
#include "stm32f10x.h"

unsigned int systicker = 0;

void delay_10us(int i){
	i*=10;
	systicker=i;
	while(systicker);
}

void SysTick_Handler(void){
	if (systicker!=0)
		systicker--;
}

void GPIOInitalize(void){
	GPIO_InitTypeDef gpio_initval;
	//Command port initalize
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	gpio_initval.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_initval.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_initval.GPIO_Pin = LCD1602_RS;
	GPIO_Init(LCD1602_RS_GROUP, &gpio_initval);
	gpio_initval.GPIO_Pin = LCD1602_RW;
	GPIO_Init(LCD1602_RW_GROUP, &gpio_initval);
	gpio_initval.GPIO_Pin = LCD1602_E;
	GPIO_Init(LCD1602_E_GROUP, &gpio_initval);

	//Data port
	//#error Be sure you have initalized related DATA pins before you remove this line of error message!!!
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF, ENABLE);
	gpio_initval.GPIO_Pin = GPIO_Pin_All;
	GPIO_Init(GPIOF,&gpio_initval);
}

void _lcd1602_write_data(unsigned short int  data){
	_LCD1602_UNSET_EN;
	_LCD1602_SET_RS;
	_LCD1602_UNSET_RW;
	//delay for 40us here
	delay_10us(4);
	//send command data
	LCD1602_WRITE_DATABAND( data );
	_LCD1602_SET_EN;
	//delay for at least 150ms here
	delay_10us(20);
	//stop process
	_LCD1602_UNSET_EN;
}

void _lcd1602_write_cmd(unsigned short int  data){
	_LCD1602_UNSET_EN;
	_LCD1602_UNSET_RS;
	_LCD1602_UNSET_RW;
	//delay for 40us here
	delay_10us(4);
	//send command data
	LCD1602_WRITE_DATABAND( data );
	_LCD1602_SET_EN;
	//delay for at least 150ms here
	delay_10us(20);
	//stop process
	_LCD1602_UNSET_EN;
}


void lcd1602_initalize(void){
	SysTick_Config(SystemCoreClock/100000);
	//Initalize related GPIO Ports
	GPIOInitalize();
	//screen reset
	_lcd1602_write_cmd( _LCD1602_CON_8BIT | _LCD1602_CON_2L | _LCD1602_CON_507 );	
	//cls if need
	#ifdef LCD1602_INIT_CLS
	lcd1602_cls();
	#endif
	//init default settings
	lcd1602_set_options(LCD1602_INIT_OPTIONS);	
}

void lcd1602_cls(void){	
	_lcd1602_write_cmd(_LCD1602_CLS);
}

void lcd1602_cursor_move(unsigned int move){	
	_lcd1602_write_cmd(move);
}

void lcd1602_cursor_pos(unsigned short int line, unsigned short int pos){
	_lcd1602_write_cmd( _LCD1602_ADDR_DDRAM( 
			(line<LCD1602_LINE_COUNT?line<<6:(line%LCD1602_LINE_COUNT)<<6) + 
			(pos<LCD1602_CHAR_COUNT?pos:pos%LCD1602_CHAR_COUNT) ));
}

void lcd1602_cr(void){
	_lcd1602_write_cmd(_LCD1602_CR);
}

void lcd1602_set_options(unsigned int config_value){
	if (_LCD1602_GET_CURSOR_CONFIG(config_value))
		_lcd1602_write_cmd(_LCD1602_GET_CURSOR_CONFIG(config_value));
	if (_LCD1602_GET_DISP_CONFIG(config_value)){
		delay_10us(20);//wait till still
		_lcd1602_write_cmd(_LCD1602_GET_DISP_CONFIG(config_value));
	}
}

void lcd1602_putchar(char ch){
	_lcd1602_write_data(ch);
}

void lcd1602_putstring(char str[]){
	char* cur = str;
	while(cur - str < 0x20 && *cur!='\0'){
		if ( cur - str == LCD1602_CHAR_COUNT)
			lcd1602_cursor_pos(1,0);
		if (*cur=='\n'){
			lcd1602_cursor_pos(1,0);
			cur++;
		}
		else
			lcd1602_putchar(*cur++);
	}
}
