#include <c8051f020.h>
#include <intrins.h>

#include "Mesure_courant.h"
#include "Config_ADC.h"

#define SYSCLK 22118400L


int Courant_ADC(void){
	// Au préalable, on envoie la tension de sortie de l'AO sur l'entrée AINO.0 (Broche A31 du 8051F020)
	//Cette tension doit être inférieure à 3.3V, il faut donc faire attention au gain de l'AO.
	float tension_AO;
	int courant;
	int gain_AO = 101;
	float R_SHUNT = 50*0.001;

	tension_AO = (ACQ_ADC()*3.3)/0xFF; // On appelle la fonction ACQ_ADC() pour récupérer la conversion (Tension en V)
	courant = (int) (tension_AO*1000)/(gain_AO*0.32*R_SHUNT); //Valeur du courant en mA

	return courant;
}
