#include <c8051f020.h>
#include <intrins.h>

#include "Transmission_UART0.h"

char Putchar(char c, unsigned char csg_tempo){
	int i=0;
	
	do{
		while (!TI0);
		if(TI0 == 1){
			SBUF0=c;
			TI0=0;
			return c;
		}
		else{
			for(i=0;i<300;i++){//Temporisation de 100us
				_nop_ ();
			}
			csg_tempo--;
		}
	}while(csg_tempo > 0);
	
	return 0;
}


void UART_CortexM4 (unsigned char trame[]){
	unsigned char rep;
	unsigned char* p;
	unsigned char csg_tempo = 100;
	
	for (p = trame; p < trame + 7;p=p+2){	
		rep = Putchar(*p,csg_tempo);
	}
	
	rep = Putchar(0xFF,csg_tempo); //Envoie de cet octet au CortexM4 pour lui dire que l'on a fini d'�mettre
}