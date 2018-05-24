#include <c8051f020.h>

#include "Timers.h"
#include <math.h>
#include <stdio.h>
#include <intrins.h>

#define SYSCLK 22118400L


void Config_Timer2 (void){
	TR2 = 0; //Désactivation du Timer2 ; Bit2 du registre T2CON
	
	CKCON &= ~0x20; //CLK_Timer2 = sysclk/12 ; Mise à '0' du bit 5 du registre CKCON, les autres bits sont à '1' 
	
	TF2 = 0; //Aucun flag d'Overflow
	EXF2 = 0; //Aucun flag extern d'Overflow, pour éviter une interruption intempestive
//Configuration du mode 1 de Timer2 : 
	RCLK0 = 0;
	TCLK0 = 0;
	
	T2CON &= ~(0x03);
 //Configuration en mode Timer
	EXEN2 = 0; //Dévalidation de l'entrée T2EX

//Programmation des registres de rechargement 
	// A MODIFIER
	RCAP2L = 0x00;
	RCAP2H = 0x00;

//Initialisation des registres de comptage du Timer 
	TL2 = 0xFF;
	TH2 = 0xFF;

//Haute prioritée du Timer2 
	PT2 = 1; //Bit 5 du registre IP

//Activation des interruption
	ET2 = 1; //Bit 5 du registre IE
	
	TR2 = 0;
}

void InitTimer0(void){
	TR0 = 0;
	TMOD |= 0x01; // 16 bit auto-reload counter
	TL0 = 0xFF;
	TH0 = 0xFF;
	
	ET0 = 1;
	PT0 = 1;
}

void Config_Timer3(void){
	int recharge;
	
	TMR3CN &= 0x00; // Timer3 disable ; Flag d'OverFlow RAZ ; Use Clock/12
	recharge = ceil(65535 - 10*0.001*SYSCLK/12); // Interruption toutes les 10 ms
	TMR3RLL = recharge;
	TMR3RLH = recharge >> 8;
	TMR3L = 0xFF;
	TMR3H = 0xFF;
	EIE2 |=0x01;
	EIP2 |= 0x01;
	TMR3CN |= 0x04; // Timer3 enable
}