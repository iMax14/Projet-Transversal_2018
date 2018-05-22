#include <c8051f020.h>
#include <math.h>
#include <stdio.h>
#include <intrins.h>

#include "Servomoteur_Vertical.h"

#define SYSCLK 22118400L



char CDE_Servo_V(signed int Angle){
	//D�claration des variables
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
	while(i < 50){ //Cette boucle permet de laisser le temps au servomoteur de se mettre en position (= 50 interruptions)
		if (TF2 == 1){
			i++;
		}
	}
	TR2 = 0; //Timer2 d�sactiv� pour le servomoteur vertical

	return Angle;
}
