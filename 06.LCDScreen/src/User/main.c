#include "stm32f10x.h"
#include "lcd1602_driver.h"
#include "serial_debugger.h"
#include "heartrun_mon.h"

int main(void){
	unsigned int i = 0;
	//Initalize On-chip Periphals
	serialdbg_init();
	heartrun_mon_init();
	//wait for battery
	for ( i = 0 ; i < 0xffff; i++);
	lcd1602_initalize();
	
	serialdbg_printstr("\n\n");
	serialdbg_printstr("This is a serial commander!\n");
	serialdbg_printstr("Type and I'll tell you what you've typed!\n\n");
	
	lcd1602_putstring("Boom shakalaka!\n2016-06-28");
	
	while(1);

}






