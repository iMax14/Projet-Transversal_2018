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
	
	int xdata i=0;
	OSCXCN = 0x67; // Crystal oscillator Mode - f > 6.7 MHz
	for (i=0;i < 3000;i++); // On attend 1ms pour l'initialisation
	while ((OSCXCN & 0x80) == 0); // On attend qu l'oscillateur à quartz soit stable
	OSCICN |= 0x08; // Basculement sur l'oscillateur externe
}

void Reset_src(void){
	//Configuration du Reset Logiciel
	RSTSRC &= 0x10; // Ici RSTSRC = 00010000
}

void Disable_WDT(void){
	//D�sactivation du Watchdog
	WDTCN = 0xde;   
	WDTCN = 0xad;
}


void GPIO_Config (void){
//Configuration du CrossBar
	
	XBR0 = 0x0E; //00001110 (XBR0.1/2/3) -- CEX0 (Pin du PCA) est rout�esur la pin P1.2
	XBR2 = 0x44; //01000100 On active l'UART1 (RX/TX pins) via le bit 2 et on active la CrossBar via le bit 6.

//Configuration des SORTIES en PUSH-PULL
	
	P0MDOUT |= 0x09; //00001001 ----- P0.3
	P1MDOUT |= 0x01; //00000001 ----- P1.2
	P2MDOUT |= 0x03; //00100011 ----- P2.0/1/5
	
	//PO.O : TX0 ; P0.3 : MISO_SPI 
	//P1.0 : CEX0 (PCA pour le pointeur lumineux)
	//P2.0 : Servo vertical
	//P2.1 : Pointeur lumineux
	
//Configuration des ENTREES en DRAIN OUVERT

	P0MDOUT &= 0xCB; //11001011 ----- P0.2/4/5	
	P0 |= 0x30; //00110000 ----- P0.4/5

	//P0.2 : SCK_SPI - P0.4 : MOSI_SPI - P0.5 : NSS_SPI
}





