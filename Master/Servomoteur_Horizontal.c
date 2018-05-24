#include <c8051f020.h>
#include <math.h>
#include <stdio.h>
#include <intrins.h>

#include "Servomoteur_Horizontal.h"

#define SYSCLK 22118400L


signed int CDE_Servo_H(signed int Angle){
	// D�claration des variables
	int rech_int;
	int i=0;
	float temps_imp;
	
	temps_imp = 0.01*Angle + 1.5; //Temps de l'impulsion en ms
	rech_int = ceil(65535 - temps_imp*0.001*SYSCLK/12);
	TR2 = 0; //Timer2 d�sactiv�
	
	//Programmation des registres de rechargement 
	RCAP2L = rech_int ;
	RCAP2H = rech_int >> 8;
	
	TR2 = 1; //Timer2 activ�
	while(i < 70){
		if (TF2 == 1){
			i++;
		}
	}//Comment� pour les tests au simulateur 
	TR2 = 0; //Timer2 d�sactiv� pour le servomoteur horizontal
	
	return Angle;
}