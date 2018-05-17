#include <c8051f020.h>
#include <intrins.h>

#include "Config_UART1.h"


#define SYSCLK 22118400UL
#define Baudrate_UART1 115200UL



void Config_Clock_UART1(void){
	T4CON &= 0x00; // On désactive le Timer4






	T4CON |= 0x04; // On désactive le Timer4


}

void Config_UART1(void){
	SCON1 |= 0x40; //COnfiguration de l'UART1 en  mode 1
	SCON1 |= 0x60; //On met un bit de stop
	SCON1 |= 0x70; //On active la r�ception
}
