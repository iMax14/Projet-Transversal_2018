#include <c8051f020.h>
#include <intrins.h>

#include "Config_UART0.h"


#define SYSCLK 22118400UL
#define Baudrate_UART2 115200UL


void Config_Clock_UART0(void){
	CKCON |= 0x10; //Sélectionne le timer 1
	TMOD |= 0x20; // Timer 1 configuré en timer 8 bits avec auto-rechargement
	TMOD &= 0x2f; // idem
	TL1=0x00; // Non nécessaire
	TH1= 256UL - SYSCLK/(16UL*Baudrate_UART2) ; // car on a une SYSLCLOCK de 22.1184MHz et un Baud Rate de 115200 --- 256 - (SYSCLK/BAUDRATE*16)
	ET1 = 0; //Désactivation des intéruption de Timer1 (Registre IE)
	TR1 = 1 ; // Registre TCON, activation du timer 1
}

void Config_UART0(void){
	TCLK0=0; // Utilisation de timer 1 comme source d'horloge (Transmission de la src d'horloge)
	RCLK0=0; // Réception de la source d'horloge
	PCON |= 0x80; // SMOD0=1 On ne divise pas le Baud rate de l'uart0 par 2
	PCON &=0xBF; //SSTAT0=0  Donne l'accès à SM20 - SM000
	SCON0 = 0x70; // Mode1 (Uart 8 bits) avec le bit 7 à 0 et le 6 à 1 ; Bit 5 à 1 pour checker le bit de stop ;
								// Bit 4 à 1 pour activer la réception de l'UART0
	TI0=1; //Pour permettre la transmission du 1er caractère
}

