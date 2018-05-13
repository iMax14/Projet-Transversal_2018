#include <c8051f020.h>
#include <intrins.h>
#include <string.h>

#include "Config_SPI.h"
#include "Declarations_GPIO.h"

#define SYSCLK 22118400L

void Config_SPI_SLAVE(void){
	//Registre SPI0CN :
	SPIEN = 0; // Disable SPI
	MSTEN = 0; // C�t� SLAVE
	RXOVRN = 0; // Si on n'a pas trait� le paquet N-1
	MODF = 0; // Pr�venir qu'il y a une collision entre le faite que la carte soit master et que NSS = 0
	WCOL = 0; // Si on a essayer de lire le registre de r�ception pendant un transfert de donn�es en cours
	SPIF = 0; // Flag d'interruption SPI0 
	//Fin de registre SPI0CN
	
	SPI0CFG |= 0x80;
	
	SPI0CKR = 0x04; // Fr�qeunce de transmission d'environ 200 kHz
	
	EIE1 |= 0x01; // Active l'interruption SPIF
	EIP1 |= 0x01; // Priorit� �lev�e
	
	SPI0DAT = 0xEE; //Initialisation du registre de rechargement de la liaison SPI
	
	SPIEN = 1; // Enable SPI
}
