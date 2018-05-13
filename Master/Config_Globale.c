#include <c8051f020.h>

#include "Config_Globale.h"

#define SYSCLK 22118400L

void Init_Device(){
	Reset_src();
	Disable_WDT();
	Oscillator_Init();
	GPIO_Config ();
}

void Oscillator_Init(){
	//Fonction d'initialisation de l'oscillateur de Quartz
	int i;
	
	OSCXCN = 0x67; // Crystal oscillator Mode - f > 6.7 MHz
	for (i=0;i < 3000;i++); // On attend 1ms pour l'initialisation
	while ((OSCXCN & 0x80) == 0); // On attend qu l'oscillateur Ã  quartz soit stable
	OSCICN |= 0x08; // Basculement sur l'oscillateur externe
}

void Reset_src(void){
	//Configuration du Reset Logiciel
	RSTSRC &= 0x10; // Ici RSTSRC = 00010000
}

void Disable_WDT(void){
	//Désactivation du Watchdog
	WDTCN = 0xde;   
	WDTCN = 0xad;
}


void GPIO_Config (void){
// Configuration du CrossBar
	
	XBR0 = 0x06; //00000110 (XBR0.1/2)
	XBR2 = 0x44; //01000100 On active l'UART1 (RX/TX pins) via le bit 2 et on active la CrossBar via le bit 6.

//Configuration des SORTIES en PUSH-PULL
	P0MDOUT |= 0x55; //01010101 ----- P0.0/2/4
	P2MDOUT |= 0x35; //00110101 ----- P2.0/1/5
	
	//P0.0 : TX0 - P0.2 : SCK_SPI - P0.4 : MOSI_SPI - P0.6 : TX1
	//P2.0 : Servo horizontal - P2.2 : Echo_Capteur_ultrason_AV - P2.4 : Echo_Capteur_ultrason_AV - P2.5 : NSS_Slave
	
//Configuration des ENTREES en DRAIN OUVERT

	P0MDOUT &= 0xD7; //11010111 ----- P0.3/5
	P2MDOUT &= 0xF5; //11110101 ----- P2.1/3
	
	P0 |= 0x28; //00101000 ----- P0.3/5
	P2 |= 0x0A; //00001010 ----- P2.1/3

	//P0.3 : MISO_SPI - P0.5 : NSS_SPI
	//P2.1 : Trig_Capteur_ultrason_AV - P2.3 : Trig_Capteur_ultrason_AV
}




