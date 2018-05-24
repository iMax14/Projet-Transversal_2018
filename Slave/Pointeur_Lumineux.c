#include <c8051f020.h> 
#include <math.h>
#include <stdio.h>
#include <intrins.h>

#include "Declarations_GPIO.h"
#include "Pointeur_Lumineux.h"

#define SYSCLK 22118400L


// Global variables
extern int cpt;
unsigned int duty;
//controle du clignotement du pointeur lumineux
void cycleAllumageExtinction(int intensite, float dureeAllumage, float dureeExtinction, int nombreCycle)
{
	int i=0;

	while(i<=nombreCycle){
		
		cpt=0;
		Allumage_Pointeur(intensite); // Allumage de la LED 
		while(cpt*10*0.001<=dureeAllumage/10); // On attend que le Timer3 compte jusqu'à dureeAllumage
		cpt=0;
		Allumage_Pointeur(0x0000); // Exctinction de la LED 
		while(cpt*10*0.01<=dureeExtinction/10); // On attend que le Timer3 compte jusqu'à dureeExtinction
		i++;
	}
}


void Allumage_Pointeur (unsigned int intensite){
	unsigned int duty;
	
	duty = 0XFFFF - 0xFFFF*(float)intensite/100;

	PCA0CPL0 = duty;
	PCA0CPH0 = duty >> 8;
}