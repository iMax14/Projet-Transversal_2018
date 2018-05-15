#include <c8051f020.h>
#include <intrins.h>

#include "ADC.h"

#define SYSCLK 22118400L


void CFG_VREF(void){
	REF0CN |= 0x03; //On active la source de tension de référence interne sur le pin VREF0
}

void CFG_ADC0(void){

	AD0EN = 0; // Désactivation de l'ADC0

	AMX0CF &= 0x00; //Configuration uniopolaire sur AIN0
	AMX0SL |= 0x00; //Conversion sur la voie AINO.3
	ADC0CF = 0x50; //Clock du convertisseur de 2,5 MHz
	ADC0CF |= 0x00; //Gain de 1

	AD0INT = 0; //On efface le drapeau signalant la fin de la conversion

	//La conversion sera déclanchée par une mise à '1' de AD0BUSY
	AD0CM1 = 0;
	AD0CM0 = 0;

	AD0LJST = 1; //Remplissage de AD0 à gauche

	AD0EN = 1; //Activation de l'ADC0
}


unsigned char ACQ_ADC(void){
	AD0INT = 0;//On remet à '0' le drapeau signalant la fin de la conversion
	AD0BUSY = 1; //On déclanche la conversion à l'aide de ce drapeau - il est automatiquement remis à '0' à la fin
	while(!AD0INT); //On attend la fin de la conversion à l'aide du flag AD0INT
	return ADC0H; // On retourne le résulat qui est sur 12 bits
}

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
