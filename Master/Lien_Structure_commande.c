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
// D�clarations des variables globales
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
	else if (commande.Etat_Energie==Mesure_I){
		*type=Courant;
	}
	else if (commande.Etat_Energie==Mesure_E){
		*type=Energie;
	}
	else if ( commande.frequence!=0){
		*type=Gene_Son;
	}
}
// verifier attention effectuer les d�claration avant toute execution de code.
void fonctionRoutage(struct COMMANDES commande){
	enum Routage route = Initialisation;
	unsigned char commande_SPI = 0x00;
	unsigned char trame[1] = {0x00};
	unsigned char taille_trame = 1;
	double alpha;
	int distance;
	char angle_ascii[3];
	char mess[50] = {0};
	char mess1[50] = {0}; // utilis� dans le cas du d�placement �l�mentaire
	char mess2[50] = {0}; // idem
	char mess3[50] = {0}; // idem
	int compteur = 0;
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
      if (commande.Servo_Angle < 0){
        trame[1] = 0xAA; //Pour prévenir le Slave que l'angle qu'il va recevoir est négatif
      }
      else{
        trame[1] = 0xBB;
      }
			taille_trame = 2;
			echange_trame(trame,taille_trame,commande_SPI);
			tempo_emiss();
			msg_Slave = trame_recue();
			tempo_emiss();
			sprintf(msg_Slave_ascii,"%f",msg_Slave);
			serOutstring("\n\r AS V");
			serOutstring("\n\r>");
			break;

		case Deplacement:
			if (commande.Etat_Mouvement == Depl_Coord) {
				alpha = atan(commande.Coord_Y / commande.Coord_X) * 180/3.1415; // r�sultat de atan en radian
				distance = ceil(pow(pow(commande.Coord_Y,2)+pow(commande.Coord_X,2),0.5)); // Pythagore
				if (alpha < 0){
					alpha+= 360; }
				// !!! L'ordre de cr�ation des messages est important (�crasement de variable)
				// instruction pour faire avancer le robot de la distance "distance"
				commande.Etat_Mouvement = Depl_Coord;
				commande.Coord_X = distance;
				commande_serializer = transcode_commande_to_serializer(commande);
				formate_serializer(commande_serializer, mess2);

				// Instruction	pour positionner le robot � l'angle finale
				commande.Etat_Mouvement = Rot_AngD;
				commande.Vitesse = 5;
				commande_serializer = transcode_commande_to_serializer(commande);
				formate_serializer(commande_serializer, mess3);

				// instruction pour positionner le robot dans l'angle de d�part
				commande.Etat_Mouvement = Rot_AngD;
				commande.Vitesse = 5;
				commande.Angle = alpha;
				commande_serializer = transcode_commande_to_serializer(commande);
				formate_serializer(commande_serializer, mess1);

				serOutstring1(mess1);
				serOutstring("\r\n 1er com pppppppppppppppppppppppppppppp\r\n");
				serOutstring(mess1);
				compteur = 0;
//				for(compteur = 0; compteur<65535; compteur++); //temporisation
				compteur=0;
				do {
					serOutstring1("pids\r"); // Attente que le serializer est fini (il renvoie 1 quand occup� et 0 sinon
						do{
							a=serInchar1();
							if (a!=0x00){
								mess[compteur]=a;
								compteur=compteur+1;
									}
							}while(a!=0x3E);
						}while ( mess[compteur-1] == 1);
										compteur = 0;

				serOutstring("\r\n 2em com \r\n");
				serOutstring1(mess2);
				serOutstring(mess2);

//				for(compteur = 0; compteur<65535; compteur++); //temporisation
				compteur=0;
				do {serOutstring1("pids\r");
						do{
							a=serInchar1();
							if (a!=0x00){
								mess[compteur]=a;
								compteur=compteur+1;
								}
							}while(a!=0x3E);
						}while ( mess[compteur-1] == 1);
											compteur = 0;

				serOutstring("\r\n 3em com \r\n");
				serOutstring1(mess3);
				serOutstring(mess3);

			}
			else
			{
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
			}


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
