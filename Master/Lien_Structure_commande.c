#include <c8051f020.h>
#include <string.h> 
#include <stdio.h>
#include <intrins.h>

#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "ADC.h"
#include "Servomoteur_horizontal.h"
#include "ultrason.h"
#include "Fonctions_cote_serializer.h"
#include "Declarations_GPIO.h"
#include "Config_Globale.h"
#include "Timers.h"
#include "Config_SPI.h"
#include "Transmission_SPI.h"
#include "UART0_RingBuffer_lib.h"
#include "ConfigUART1.h"
#include "Lien_Structure_commande.h"
#include "encodage_son.h"
#include "son_sonore.h"
#include "mesure_courant.h"

#ifndef CFG_Globale
  #define CFG_Globale
  #include <CFG_Globale.h>
#endif

//------------------------------------------------------------------------------------
// Déclarations des variables globales
//------------------------------------------------------------------------------------

char a;
char i =0;
char message_s[50] = {0};
char mess[50] = {0};
void tempo_emiss(void);
struct COMMANDES_SERIALIZER commande_serializer;
float courant_actuel;
float dist_avant;
float dist_arriere;
char Angle_voulu,Angle_atteint,msg_Slave,angle;

void routage(struct COMMANDES commande, enum Routage * type){
			
	if (commande.Etat_Servo==Servo_H){
		*type=Servo_Hor;
	}		
	else	if (commande.Etat_Servo==Servo_V){
		*type=Servo_Vert;
	}					
	else if (commande.Etat_Mouvement!=Mouvement_non){
		*type=Deplacement;
	}
	else if (commande.A_Obst!=DCT_non){
		*type=Obstacle;
	}
	else if (commande.Etat_Energie!=Mesure_I){
		*type=Courant;
	}
	else if (commande.Etat_Energie!=Mesure_E){
		*type=Energie;
	}
	else if ( commande.frequence!=0){
		*type=Gene_Son;
	}
}
// verifier attention effectuer les déclaration avant toute execution de code.		
void fonctionRoutage(struct COMMANDES commande){
	enum Routage route = Initialisation;
	unsigned char commande_SPI = 0x00;
	unsigned char trame[1] = {0x00};
	unsigned char taille_trame = 1;
	char angle_ascii[3];
	char mess[50] = {0};
	char msg_Slave_ascii[256];	
  struct INFORMATIONS info;
	char courant_ascii[4];
	char energie_ascii[4];
	routage(commande,&route);
	
	switch (route){
		case Servo_H:	
			Angle_voulu=commande.Servo_Angle;
			Angle_atteint = CDE_Servo_H(Angle_voulu);
			serOutstring("\n\r AS H");
			
			memset(mess,0,strlen(mess));
			strcpy(mess,angle_ascii);
			strcat(mess,"\n\r>");
			serOutstring(mess);
			break;
		
		case Servo_V:
			commande_SPI = 0xD3;
			trame[0]=commande.Servo_Angle;
			taille_trame = 1;
			echange_trame(trame,taille_trame,commande_SPI);
			tempo_emiss();
			msg_Slave = trame_recue();
			tempo_emiss();

			sprintf(msg_Slave_ascii,"%f",msg_Slave);
			serOutstring("\n\r AS V");
			serOutstring("\n\r>");
			break;
		
		case Deplacement:	
			commande_serializer = transcode_commande_to_serializer(commande);
			formate_serializer(commande_serializer, message_s);
			serOutstring1(message_s);
			serOutstring(message_s);

			i=0;
			a=0;
			do{	
				a=serInchar1();
				if (a!=0x00){
					mess[i]=a;
					i=i+1;
				}
			}while(a!=0x3E);
				
			mess[i] = '\0';
			serOutstring(mess);
			break;
		
		case Obstacle:
			memset(mess,0,strlen(mess));
			info = encode_son(commande);
			strcpy(mess,"\n\rDistance (cm) : ");
			serOutstring(mess);
			serOutstring(info.Tab_Val_Obst);
			serOutstring("\n\r>");
			memset(mess,0,strlen(mess));
			break;
	
		case Courant:
			info.Mesure_Courant = Courant_ADC();
			sprintf( courant_ascii,"%d", info.Mesure_Courant);
			serOutstring(courant_ascii);
			serOutstring("mA\n\r>");
			break;
		case Energie : 
			info.Mesure_Courant = Courant_ADC();
			info.Mesure_Energie = (int) 9.6*info.Mesure_Courant*2; // E = U*I*t
			sprintf(energie_ascii,"%d", info.Mesure_Energie);
			serOutstring(energie_ascii);
			serOutstring("J\n\r>");
			break;
		
		case Gene_Son:
			son_sonore(commande);
			break;
		
		default:
			strcpy(mess, "\n\r#\n\r>");
			serOutstring(mess);
			break;
		
	}
}

void tempo_emiss(void){	
	unsigned int x = 0;
	
	for(x=0;x<65000;x++);
	for(x=0;x<65000;x++);
	for(x=0;x<65000;x++);
	for(x=0;x<65000;x++);
}
