#ifndef __LCD1602_DRV_H__
#define __LCD1602_DRV_H__

#include "lcd1602_config.h"

/* Cursor movement commands */
#define LCD1602_CURSOR_LEFT 0x10
#define LCD1602_CURSOR_RIGHT 0x14

//Followings are used only if your screen support move
#ifdef LCD1602_CAN_SCREENMOVE
#define LCD1602_SCREEN_LEFT 0x18
#define LCD1602_SCREEN_RIGHT 0x1C
#define LCD1602_SCREEN_NOAUTOMOV (0x4<<__LCD1602_CURSOR_CFG_OFFSET)
#define LCD1602_SCREEN_AUTORIGHT  (0x5<<__LCD1602_CURSOR_CFG_OFFSET)
#endif

/* Those configure values should be bitor*/
// Cursor/Screen mov config
#define LCD1602_CURSOR_AUTOLEFT   (0x4<<__LCD1602_CURSOR_CFG_OFFSET)
#define LCD1602_CURSOR_AUTORIGHT  (0x6<<__LCD1602_CURSOR_CFG_OFFSET)
#define __LCD1602_CURSOR_CFG_OFFSET 0
#define _LCD1602_GET_CURSOR_CONFIG(config) (((config)>>__LCD1602_CURSOR_CFG_OFFSET)&0x7)
// Display Config
#define LCD1602_DISP_OFF     (0x8<<__LCD1602_DISP_CFG_OFFSET)
#define LCD1602_DISP_ON      (0xC<<__LCD1602_DISP_CFG_OFFSET)
#define LCD1602_CURSOR_OFF   (0x8<<__LCD1602_DISP_CFG_OFFSET)
#define LCD1602_CURSOR_ON    (0xA<<__LCD1602_DISP_CFG_OFFSET)
#define LCD1602_CURSOR_STILL (0x8<<__LCD1602_DISP_CFG_OFFSET)
#define LCD1602_CURSOR_BLINK (0x9<<__LCD1602_DISP_CFG_OFFSET)
#define __LCD1602_DISP_CFG_OFFSET 3
#define _LCD1602_GET_DISP_CONFIG(config) (((config)>>__LCD1602_DISP_CFG_OFFSET)&0xF)

/* Controller Configs which will not merged with others */
#define _LCD1602_CON_4BIT		 0x20
#define _LCD1602_CON_8BIT		 0x30
#define _LCD1602_CON_1L			 0x20
#define _LCD1602_CON_2L			 0x28
#define _LCD1602_CON_507		 0x20
#define _LCD1602_CON_510		 0x24

/* Macros used to operate function lines */
//chip set=enable unset=disable
#define _LCD1602_SET_EN GPIO_SetBits(LCD1602_E_GROUP, LCD1602_E);
#define _LCD1602_UNSET_EN GPIO_ResetBits(LCD1602_E_GROUP, LCD1602_E);
//chip set=read unset=write
#define _LCD1602_SET_RW GPIO_SetBits(LCD1602_RW_GROUP, LCD1602_RW);
#define _LCD1602_UNSET_RW GPIO_ResetBits(LCD1602_RW_GROUP, LCD1602_RW);
//chip set=data unset=command
#define _LCD1602_SET_RS GPIO_SetBits(LCD1602_RW_GROUP, LCD1602_RS);
#define _LCD1602_UNSET_RS GPIO_ResetBits(LCD1602_RW_GROUP, LCD1602_RS);

/* Some fixed value of command */
#define _LCD1602_CLS 0x1
#define _LCD1602_CR 0x2

/* Macros */
//convert value to CGRAM or DDRAM address
#define _LCD1602_ADDR_CGRAM(data) (((data)&0x3F)|0x40)
#define _LCD1602_ADDR_DDRAM(data) (((data)&0x7F)|0x80)
//abstract value from read value
#define _LCD1602_CONVERT_BUSY(data) ((data)&0x80)
#define _LCD1602_CONVERT_ACPOS(data) ((data)&0x7F)

//Initalization
void lcd1602_initalize(void);

//Clean the screen
void lcd1602_cls(void);

//Carrage return to left
void lcd1602_cr(void);

//Set config value with LCD1602_CFG values
void lcd1602_set_options(unsigned int config_value);

//put a char
void lcd1602_putchar(char ch);

//put a string
void lcd1602_putstring(char str[]);

//use macro define to do cursor move
void lcd1602_cursor_move(unsigned int move);


#endif
