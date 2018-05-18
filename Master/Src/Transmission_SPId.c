#include <c8051f020.h>
#include <intrins.h>
#include <string.h>

#include "Transmission_SPI.h"
#include "Declarations_GPIO.h"

#define SYSCLK 22118400L


void trame_emise(unsigned char trame)
{
	unsigned int i;
	for (i = 0; i<65000;i++);
	
	SPIF = 0;
	NSS_slave = 0;
	SPI0DAT = trame; // données envoyées

	while(SPIF == 0){}
	
	NSS_slave = 1;	
	SPIF = 0;
}


unsigned char trame_recue(void){
		unsigned char ret;
		
		return ret = SPI0DAT;	
}

// Fonction à modifier !!!
void debut_trame(void)
{
	//0xD1 : Cortex M4 ; 0xD2 : Pointeur lumineux  
	//0xD3 : Servomoteur vertical ; 0xD4 : Prise de vue ; 0xD5 : FPGA
	trame_emise(0xD3);
	trame_emise(0xD3);

}


unsigned char echange_octet(unsigned char octet)
{
	unsigned char ret;
	trame_emise(octet);
	ret = trame_recue();
	
	return ret;

}

void fin_trame(void)
{	
	trame_emise(0xFF);
	trame_emise(0xFF);
}

void echange_trame()
{
	unsigned char trame[1]= {0x5A};
	unsigned char rep;
	unsigned char* p ;
	int i = sizeof(trame);

	debut_trame();
	
	for (p = trame; i > 0;i--,p++)
	{	
		rep = echange_octet(*p);

	}
	
	fin_trame();
	NSS_slave = 1;

}
