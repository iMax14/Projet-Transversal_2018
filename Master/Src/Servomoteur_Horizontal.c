#include <c8051f020.h>
#include <math.h>
#include <stdio.h>
#include <intrins.h>

#include "Servomoteur_Horizontal.h"

#define SYSCLK 22118400L


char CDE_Servo_H(signed int Angle){
	// Déclaration des variables
	int rech_int , i=0;
	float temps_imp;
	
	temps_imp = 0.01*Angle + 1.5; //Temps de l'impulsion en ms
	rech_int = ceil(65535 - temps_imp*0.001*SYSCLK/12);
	TR2 = 0; //Timer2 désactivé
	
	//Programmation des registres de rechargement 
	RCAP2L = rech_int ;
	RCAP2H = rech_int >> 8;
	
	TR2 = 1; //Timer2 activé
	/*while(i < 50){
		if (TF2 == 1){
			i++;
		}
	}Commenté pour les tests au simulateur */
	TR2 = 0; //Timer2 désactivé pour le servomoteur horizontal
	
	return Angle;
}