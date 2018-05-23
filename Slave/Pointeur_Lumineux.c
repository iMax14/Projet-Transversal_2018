#include <c8051f020.h> 
#include <math.h>
#include <stdio.h>
#include <intrins.h>

#include "Declarations_GPIO.h"
#include "Pointeur_Lumineux.h"

#define SYSCLK 22118400L


// Global variables
unsigned char PWM=0;  // varie entre 0 et 255. permet de choisir le duty cycle
unsigned int temp=0;    // utilisé dans le timer0
extern int cpt;
int duty;

//controle du clignotement du pointeur lumineux
void cycleAllumageExtinction(int intensite, float dureeAllumage, float dureeExtinction, int nombreCycle)
{
	int i=0;
	int rech;

	while(i<nombreCycle){
		duty = (18*intensite)/50;//Intensité = 50% => duty = 18 ms
		
		rech = ceil(65535 - duty*0.001*SYSCLK/12);
		TH0  = rech >> 8; 
		TL0  = rech;
		TR0 = 1;

		cpt=0;
		while(cpt*10<=dureeAllumage); // On attend que le Timer3 compte jusqu'à dureeAllumage
		cpt=0;
		while(cpt*10<=dureeExtinction); // On attend que le Timer3 compte jusqu'à dureeExtinction
		i++;
	}
}