#include <c8051f020.h>
#include <string.h> 
#include <stdio.h>
#include <intrins.h>

#include "Declarations_GPIO.h"
#include "Config_Globale.h"
#include "Timers.h"
#include "Servomoteur_Horizontal.h"
#include "ultrason.h"
#include "Config_SPI.h"
#include "Transmission_SPI.h"
#include "ADC.h"
#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "UART0_RingBuffer_lib.h"
#include "ConfigUART1.h"
#include "UART0_RingBuffer_lib.h"
#include "Fonctions_cote_serializer.h"

int conversion_courant (unsigned char tableau[4]){
	int mille=0;
	int centaine=0;
	int dizaine=0;
	int unite=0; 
	int valeur;
	
	mille= (tableau[0]-'0')*1000;
	centaine= (tableau[1]-'0')*100;
	dizaine= (tableau[2]-'0')*10;
	unite= (tableau[3]-'0');
	
	valeur=mille+centaine+dizaine+unite;
	return valeur	;
}

struct INFORMATIONS mesure_courant(void){
	float courant;
	char courant_ascii[4];
	int valeur_courant;
	struct INFORMATIONS info;
	courant=Courant_ADC();
	sprintf( courant_ascii,"%f", courant);
	valeur_courant= conversion_courant(courant_ascii);
	
	if (valeur_courant !=0){
		info.Etat_RESULT_Courant=RESULT_Courant_oui;
		info.Mesure_Courant=valeur_courant;
	}
	else{
		info.Etat_RESULT_Courant=RESULT_Courant_non;
	}
	return info; 
}

