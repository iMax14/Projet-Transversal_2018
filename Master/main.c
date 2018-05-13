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
#include "Lien_Structure_commande.h"

#ifndef CFG_Globale
  #define CFG_Globale
  #include <CFG_Globale.h>
#endif


//------------------------------------------------------------------------------------
// Déclarations des variables globales
//------------------------------------------------------------------------------------
int vitesse_par_defaut = 10;
char message_PC_com[50] = {0};
enum Epreuve epreuve_en_cours = Epreuve_non;
int commande_correct = 0;

char conversioncoord (unsigned char tableau[2]){
	int dizaine=0;
	int unite=0; 
	char valeur;
	
	dizaine= (tableau[0]-'0')*10;
	unite= (tableau[1]-'0');
	
	valeur=dizaine+unite;
	return valeur	;
}

/*char conversionvitesse (unsigned char tableau[3]){
	int centaine=0;
	int dizaine=0;
	int unite=0; 
	char valeur;
	
	centaine= (tableau[0]-'0')*100;
	dizaine= (tableau[1]-'0')*10;
	unite= (tableau[2]-'0');
	
	valeur= centaine+dizaine+unite;
	return valeur	;
}*/

signed int conversionangle_positif (unsigned char tableau[3]){ 
	signed int centaine=0;
	signed int dizaine=0;
	signed int unite=0; 
	signed int valeur;
	
	centaine= (tableau[0]-'0')*100;
	dizaine= (tableau[1]-'0')*10;
	unite= (tableau[2]-'0');

	valeur= centaine+dizaine+unite;
	return valeur	;
}

struct COMMANDES traitement_A(char * com, struct COMMANDES commande){ // traitement lorsque la première lettre est A (AVANCER A DUREE DAQUISITION) AVANCER OK
	char vitesse;
	char tab[3];
	int j;	
	char duree;

	switch (com[1]) //AJOUTER LA DUREE
			{
				case 'S': // cas ASS (Durée) 
					{
						commande_correct = 0; //TODO commande_correct = nbr pour aquisition signal
						for (j=4; j<7;j++)
							{
							tab[j-4]=com[j];
						}
						duree=conversionangle_positif(tab);
						if (duree >= 100 ||duree <1)
							{						
							commande_correct = 0;
						}
						else{commande.ACQ_Duree=duree;}
						break;
				}
				case ' ':// cas d'avancer A
				{
					if (epreuve_en_cours == epreuve1){
						commande.Etat_Mouvement=Avancer;
						// on regarde le paramètre de la vitesse
						if (com[2] >= 0x30)
						{
							for (j=2; j<5;j++)
								{
									tab[j-2]=com[j];
							}
							vitesse=conversionangle_positif(tab);
						if (vitesse >= 100 ||vitesse <=0)
							{
								commande_correct = 0;
							}
						else
							{
							commande.Vitesse=vitesse;
							commande_correct = 1; // nbr 1 pour routage vers le serializer

							}}
						else{ commande_correct = 1; // nbr 1 pour routage vers le serializer
						}
					}
							else {
								commande_correct = 2;
								strcpy(message_PC_com,"\r\nErreur non epreuve 1\r\n>");}
					break;
				}
			default:
				{ 
				if (epreuve_en_cours == epreuve1){
					commande.Etat_Mouvement=Avancer;
					commande_correct = 1;}
					else{
								commande_correct = 2;
								strcpy(message_PC_com,"\r\nErreur non epreuve 1\r\n>");}
			}
		}
		return commande;
}
struct COMMANDES traitement_B(char* com, struct COMMANDES commande) // RECULER OK
{
	char vitesse;
	char tab[3];
	int j;
	commande.Etat_Mouvement=Reculer;
			if (com[1]==' ')
				{
				for (j=2; j<=4;j++)
				{
						tab[j-2]=com[j];
				}
				vitesse=conversionangle_positif(tab);
					if (vitesse >= 100 ||vitesse <0)
						{
							commande_correct = 0;
						}
					else
						{
						commande.Vitesse=vitesse;
						commande_correct = 1;
						}
				}
			
			else {
						commande_correct = 1;
			}

			return commande;
}
struct COMMANDES traitement_C(char* com, struct COMMANDES commande) // pilotage du servomoteur
{
	signed int angle;
	unsigned char tab[3];
	int j=0;
	switch (com[3])
	{
		case 'H':
		{
			commande.Etat_Servo=Servo_H;
			if (com[5]=='A')
				{
				for (j=7; j<10;j++)
					{
							tab[j-7]=com[j];
					}
				angle=conversionangle_positif(tab);
					if (angle > 90 ||angle <-90)
					{
						commande_correct = 0;
					}
					else
					{
						commande.Servo_Angle=angle;
						commande_correct = 1;
					}
			}
			else
			{
				angle=0;
			}
			commande.Servo_Angle=angle;
			break;
		}
		case 'V':
		{
			commande.Etat_Servo=Servo_V;
			if (com[5]=='A')
				{
				for (j=7; j<10;j++)
					{
							tab[j-7]=com[j];
					}
				angle=conversionangle_positif(tab);
					if (angle > 90 ||angle <-90)
					{
						commande_correct = 0;
					}
					else
					{
						commande.Servo_Angle=angle;
						commande_correct = 1;
					}
			}
			
			else
			{
				angle=0;
				commande.Servo_Angle=angle;
			}
			
			break;
		}
		default:
		{
			commande.Etat_Servo=Servo_H;
			angle=0;
			commande.Servo_Angle=angle;
			break;
		}
	}
	return commande;
}
struct COMMANDES traitement_D(char * com, struct COMMANDES commande)// DEBUT DEPREUVE OK
{
	switch (com[2]) // exemple message:  D 1 
			{
				case '1': //épreuve 1
				{
					commande.Etat_Epreuve=epreuve1;
					strcpy(message_PC_com, "\r\nInvite de commande 1\r\n>");
					epreuve_en_cours = epreuve1;
					//serOutstring("Invite de commande 1");
					break;

				}
				case '2': //épreuve 2
				{
					commande.Etat_Epreuve=epreuve2;
					strcpy(message_PC_com, "\r\nInvite de commande 2\r\n>");
					epreuve_en_cours = epreuve2;
					//serOutstring("Invite de commande 2");
					break;

				}
				case '3': //épreuve 3
				{
					commande.Etat_Epreuve=epreuve3;
					epreuve_en_cours = epreuve3;
					strcpy(message_PC_com, "\r\nInvite de commande 3\r\n>");
					//serOutstring("Invite de commande 3");
					break;

				}
				case '4': //épreuve 4
				{
					commande.Etat_Epreuve=epreuve4;
					epreuve_en_cours = epreuve4;
					strcpy(message_PC_com, "\r\nInvite de commande 4\r\n>");
					//serOutstring("Invite de commande 4");
					break;

				}
				case '5': //épreuve 5
				{
					commande.Etat_Epreuve=epreuve5;
					epreuve_en_cours = epreuve5;
					strcpy(message_PC_com, "\r\nInvite de commande 5\r\n>");
					//serOutstring("Invite de commande 5");
					break;
				}
				case '6': //épreuve 6
				{
					commande.Etat_Epreuve=epreuve6;
					epreuve_en_cours = epreuve6;
					strcpy(message_PC_com, "\r\nInvite de commande 6\r\n>");
					//serOutstring("Invite de commande 6");
					break;

				}
				case '7': //épreuve 7
				{
					commande.Etat_Epreuve=epreuve7;
					epreuve_en_cours = epreuve7;
					strcpy(message_PC_com, "\r\nInvite de commande 7\r\n>");
					//serOutstring("Invite de commande 7");
					break;
				}
				case '8': //épreuve 8
				{
					commande.Etat_Epreuve=epreuve8;
					epreuve_en_cours = epreuve8;
					strcpy(message_PC_com, "\r\nInvite de commande 8\r\n>");
					//serOutstring("Invite de commande 8");
					break;
				}
				default:
				{
				commande.Etat_Epreuve=epreuve1;
					epreuve_en_cours = epreuve1;
				strcpy(message_PC_com, "\r\nInvite de commande 1\r\n>");
				//serOutstring("Invite de commande 1");

				}
			}
			commande_correct = 2;
			return commande;
}
struct COMMANDES traitement_E(struct COMMANDES commande)//FIN DEPREUVE OK
{
	//serOutstring("Fin de l'épreuve");
	commande.Etat_Epreuve=Epreuve_non;
	commande.Etat_Mouvement =Mouvement_non;
	epreuve_en_cours = Epreuve_non;
	return commande;
}
struct COMMANDES traitement_G(char * com, struct COMMANDES commande) // DEPLACEMENT AVEC ANGLE : OK
{
	
	int cpt=0;
	int bin=1;
	signed int angle;
	int signe_angle=0; // positif si 0 négatif si 1
	int signe_x=0; // positif si 0 négatif si 1
	int signe_y=0; // positif si 0 négatif si 1
	int k;
	signed char coordx;
	signed char coordy;
	char tabcoordx[2];
	char tabcoordy[2];	
	unsigned char tabangle[3];	
	commande.Etat_Mouvement=Depl_Coord;

			for(k=0;k<=50;k++)
			{
				bin=1;
				if (com[k]==':' && cpt==0 && bin==1)
				{
					
						if (com[k+1]=='-')
						{	
							tabcoordx[0]=com[k+2];
							tabcoordx[1]=com[k+3];
							
							coordx=conversioncoord(tabcoordx);
							coordx=coordx;
							signe_x=1;
							cpt=cpt+1;
							bin=0;
							
						}
						else
						{
							tabcoordx[0]=com[k+1];
							tabcoordx[1]=com[k+2];
							coordx=conversioncoord(tabcoordx);
							cpt=cpt+1;
							bin=0;
						}
					}
					if (com[k]==':' && cpt==1 && bin==1)
						{
							if (com[k+1]=='-')
							{
								
								tabcoordy[0]=com[k+2];
								tabcoordy[1]=com[k+3];
								coordy=conversioncoord(tabcoordy);
								coordy=coordy;
								signe_y=1;
								cpt=cpt+1;
								bin=0;
							}
							else
							{
								tabcoordy[0]=com[k+1];
								tabcoordy[1]=com[k+2];
								coordy=conversioncoord(tabcoordy);
								cpt=cpt+1;
								bin=0;
							}
					}
					if (com[k]==':' && cpt==2 && bin==1)
					{
						if (com[k+1] =='-')
						{
						tabangle[0]=com[k+2]; // saut du signe -
						tabangle[1]=com[k+3];
						tabangle[2]=com[k+4];
						signe_angle=1;
						angle=conversionangle_positif(tabangle);
						}
						else
						{
						tabangle[0]=com[k+1];
						tabangle[1]=com[k+2];
						tabangle[2]=com[k+3];
						angle=conversionangle_positif(tabangle);
						}
						// DEMANDER A THibaut comment il traite G 
						switch (signe_angle)
						{
							case 1: // cas angle négatif selon le sens trigonométrique
								{
									if (signe_x==1 && signe_y==0)
									{
										angle=-angle;
										//commande.Etat_Mouvement=Rot_AngG;
									}
									if (signe_x==1 && signe_y==1)
									{
										angle=180-angle;
										//commande.Etat_Mouvement=Rot_AngG;
									}
									bin=0;
									if (signe_x==0 && signe_y==1)
									{
										angle=-(180-angle);
										//commande.Etat_Mouvement=Rot_AngD;

									}
									break;
								}
							default:// cas angle positif selon le sens trigonométrique
							{
								if (signe_x==1 && signe_y==0)
									{
										angle=-angle;
									}
								if (signe_x==1 && signe_y==1)
									{
										//commande.Etat_Mouvement=Rot_AngD;
										angle=-(180-angle);
									}
								if (signe_x==0 && signe_y==1)
									{
										//commande.Etat_Mouvement=Rot_AngG;
										angle=180-angle;
									}
								break;
							}
						}
						bin=0;
					}
			}
		commande.Coord_X=coordx;
		commande.Coord_Y=coordy;
		commande.Angle=angle;
		return commande;
}
struct COMMANDES traitement_I(char * com, struct COMMANDES commande) // pas encore utilisé
{
	// a compléter
	return commande;
}
struct COMMANDES traitement_M(char *com, struct COMMANDES commande) 
{
	int j;
	char tab[2];
	char angle;
	switch(com[1])
		{
				case 'I': //MI Mesure de courant
					commande.Etat_Energie=Mesure_I;
					break;
				case 'E': //ME Mesure de l'énergie
					commande.Etat_Energie=Mesure_E;
					break;
				case 'O': //cas MO
					switch (com[2])
					{
						case 'U': // cas MOU 
							commande.A_Obst=Obst_unique;
							switch (com[4])
							{
								case 'D':
									commande.Det=avant_arriere;//avant et arriere
									break;
								default:
									commande.Det=avant;
									//avant
									break;
							}
							break;
						case 'B': // cas MOB 
							commande.A_Obst=Obst_balayage;
							switch(com[4])
							{
								case 'A':
									commande.Etat_DCT_Obst=oui_360; // défault de D
									for (j=6; j<8;j++)
									{
										tab[j-6]=com[j];
									}
									angle=conversioncoord(tab);
									if (angle%5==0 && (angle>5 || angle<45))
									{
										commande.DCT_Obst_Resolution=angle;
										commande_correct=1;
									}
									else
									{
										commande_correct=0;
									}
									
									break;
								case 'D':
									commande.Etat_DCT_Obst=oui_180;
									if (com[6]=='A')
									{
										for (j=8; j<10;j++)
										{
											tab[j-8]=com[j];
										}
										angle=conversioncoord(tab);
										if (angle%5==0 && (angle>5 || angle<45))
										{
										commande.DCT_Obst_Resolution=angle;
										commande_correct=1;
										}
										else
										{
											commande_correct=0;
										}
									}
										
									else
									{
										commande.DCT_Obst_Resolution=30;
									}
									break;
								default:
									commande.Etat_DCT_Obst=oui_360;
									commande.DCT_Obst_Resolution=30;
									break;
							}	
						case 'S': // cas MOS 
							commande.A_Obst=Obst_balayage;
							switch(com[4])
							{
								case 'A':
									commande.Etat_DCT_Obst=oui_360; // défault de D
									for (j=6; j<8;j++)
									{
										tab[j-6]=com[j];
									}
									angle=conversioncoord(tab);
									if (angle%5==0 && (angle>5 || angle<45))
									{
										commande.DCT_Obst_Resolution=angle;
										commande_correct=1;
									}
									else
									{
										commande_correct=0;
									}
									
									break;
								case 'D':
									commande.Etat_DCT_Obst=oui_180;
									if (com[6]=='A')
									{
										for (j=8; j<10;j++)
										{
											tab[j-8]=com[j];
										}
										angle=conversioncoord(tab);
										if ((angle%5==0) && (angle>=5) && (angle<=45))
										{
											commande.DCT_Obst_Resolution=angle;
											commande_correct=1;
										}
										else
										{
											commande_correct=0;
										}
									}
										
									else
									{
										commande.DCT_Obst_Resolution=30;
									}
									break;
								default:
									commande.Etat_DCT_Obst=oui_360;
									commande.DCT_Obst_Resolution=30;
									break;
							}	
							break;
						default:break;
					}
					break;
				}
			return commande;
}
struct COMMANDES traitement_P(char *com, struct COMMANDES commande) // PAS ENCORE UTILISE (POS)
{
	// a compléter
	return commande;
}
struct COMMANDES traitement_Q(struct COMMANDES commande) //ARRET DURGENCE 
{
	//serOutstring("arret d'urgence");
	commande.Etat_Epreuve=Stop_Urgence;
	return commande;
}
struct COMMANDES traitement_R(char * com, struct COMMANDES commande)// DIFFERENTES ROTATIONS OK
{
	signed int angle;
	unsigned char tab[3];
	int j;
	switch (com[1])
			{
			case 'D': // si on recoit RD
			{
				commande.Etat_Mouvement=Rot_90D;
				break;
			}
			case 'G': //Si on recoit RG
			{
				commande.Etat_Mouvement=Rot_90G;	
				break;
			}
			case 'C':// si on recoit RC
			{
				if (com[3]== 'D')
				{
					commande.Etat_Mouvement=Rot_180D;	
					break;
				}
				if (com[3]== 'G')
				{
					commande.Etat_Mouvement=Rot_180G;	
					break;
				}
			}
			case 'A': // on recoit RA
			{
				switch (com[3])
				{
					case 'D':
					{
						commande.Etat_Mouvement=Rot_AngD;	
						for (j=5; j<=7;j++)
						{
								tab[j-5]=com[j];
						}
						angle=conversionangle_positif(tab);
						commande.Angle=angle;
						break;
					}
					case 'G':
					{
						commande.Etat_Mouvement=Rot_AngG;
						for (j=5; j<=7;j++)
						{
								tab[j-5]=com[j];
						}
						angle=conversionangle_positif(tab);
						commande.Angle=angle;
						break;
					}
					default :
					{
						commande.Etat_Mouvement=Rot_AngD;	
						angle=90;
						commande.Angle=angle;
						break;
					}
			}
			break;
		}
			default:
			{
				break;
			}
		}
		return commande;
}
struct COMMANDES traitement_S(char * com, struct COMMANDES commande) // STOPPER
{
	int i;
	char tab[2];
	char frequence;
	char duree_son;
	char duree_silence;
	char nombre_Bips;
	
	if( com[1]=='D')
	{
			if(com[3]=='F')
			{
				for (i=5; i<7;i++)
				{
					tab[i-5]=com[i];
				}
				frequence=conversioncoord(tab);
				if (frequence >99 ||frequence <1)
				{
						commande_correct = 0;
				}
				else
				{
					commande.frequence=frequence;
					commande_correct = 1;
				}
				for (i=10; i<12;i++)
				{
					tab[i-10]=com[i];
				}
				duree_son=conversioncoord(tab);
				if (duree_son >99 ||duree_son <1)
				{
						commande_correct = 0;
				}
				else
				{
					commande.duree_son=duree_son;
					commande_correct = 1;
				}
				for (i=15; i<17;i++)
				{
					tab[i-15]=com[i];
				}
				duree_silence=conversioncoord(tab);
				if (duree_silence >99 ||duree_silence <1)
				{
						commande_correct = 0;
				}
				else
				{
					commande.duree_silence=duree_silence;
					commande_correct = 1;
				}
				for (i=20; i<22;i++)
				{
					tab[i-20]=com[i];
				}
				nombre_Bips=conversioncoord(tab);
				if (nombre_Bips >99 ||nombre_Bips <1)
				{
						commande_correct = 0;
				}
				else
				{
					commande.nombre_Bips=nombre_Bips;
					commande_correct = 1;
				}
				
			}
			else
			{
				commande.frequence=6;								// définit par le code fréquence allant de 1 a 99 (mais defini de 1 a 21)
				commande.duree_son=25;									// durée du signal sonore
				commande.duree_silence=50;							// duree du silence
				commande.nombre_Bips=3;							// nombre de bips
			}
	}
	else
	{
		commande.Etat_Mouvement=Stopper;	
	}
	return commande;
}
struct COMMANDES traitement_T(char * com, struct COMMANDES commande) // VITESSE A REGLER
{
	char vitesse;
	char tab[3];
	int j;
	// on regarde le paramètre de la vitesse
			
	for (j=3; j<=5;j++)
	{
			tab[j-3]=com[j];
	}	
	vitesse=conversionangle_positif(tab);
	j = ((int)vitesse& 0x00FF);
	if (j >= 100){
			commande_correct = 0;}
	else {
			vitesse_par_defaut=(vitesse & 0x00FF);
			strcpy(message_PC_com,"\r\n>");
			commande_correct = 2;
	}
	return commande;
	
}

struct COMMANDES Message (char * com){
//	char tabcoordx[2];
//	char tabcoordy[2];	
//	char tabangle[2];	
//	signed char coordx;
//	signed char coordy;
//	char angle;
//	int cpt=0;
	struct COMMANDES commande;
	commande.Etat_Epreuve = Epreuve_non;
	commande.Vitesse = vitesse_par_defaut;
	commande.Etat_Mouvement = Mouvement_non;
	commande.Etat_ACQ_Son = ACQ_non;
	commande.A_Obst = Non_obst;
	commande.Etat_DCT_Obst = DCT_non;
	commande.Etat_Lumiere = Lumiere_non;
	commande.Etat_Servo = Servo_non;
	commande.Etat_Energie = Energie_non;
	commande.Etat_Photo = Photo_non;
	commande.Etat_Position = Position_non;
	
		switch (com[0])
		{
		case 'A':// si on recoit un A AVANCER OU DUREE DAQUISITION FIXEE
		{
			commande=traitement_A(com,commande);
			break;
		}
		case 'T':// si on recoit un TV Reglage de la vitesse seulement
		{
			commande=traitement_T(com,commande);
			break;
		}
		case 'D':// si on recoit un D (Début de l'epreuve)
		{
			commande=traitement_D(com,commande);
			break;
		}
		case 'E':// si on recoit un E (fin de l'epreuve)
		{
			commande=traitement_E(commande);
			break;

		}
		case 'Q':// si on recoit un Q (arret d'urgence)
		{
			commande=traitement_Q(commande);
			break;

		}
		case 'M':// si on recoit un M (mesure et autre) PAS UTILISE 
		{
			commande=traitement_M(com,commande);
			break;
		}
		case 'I':// si on recoit un IPO (I)
		{
			// a faire
			commande=traitement_I(com,commande);
			break;
		}
		case 'P':// si on recoit un POS 
		{
			commande=traitement_P(com,commande);
			//envoie de information
			break;

		}
		case 'B': //si on recoit un B RECULER
		{
			commande=traitement_B(com,commande);
			break;
		}
		case 'S': // si on recoit S
		{
			commande=traitement_S(com, commande);
			break;
		}
		case 'G': // si on recoit G
		{
			commande=traitement_G(com,commande);
			break;
		}
		
		case 'R': //Si On recoit R ROTATION
		{
			commande=traitement_R(com,commande);
			break;
		}
		case 'C':
			commande=traitement_C(com,commande);
			break;
		default:
		{
		commande_correct = 0;
		}
}
//}
return commande;
}

	

void main (void)
{	
//Déclaration des variables
	//struct COMMANDES_SERIALIZER commande_serializer;
	struct COMMANDES commande;
	char message_s[50] = {0};
	char mess[6] = {0};
	char echo[3] = {0};
	char com[50] = {0};
//	char nbr[10];
	char a;
	char i =0;
	int b = 0;
	unsigned char code_err = 0;


	
	EA=0;
	Init_Device();  // Initialisation du microcontrôleur
	Config_Timer2();
	Config_timer0();
	Config_SPI_MASTER();
	CFG_VREF();
	CFG_ADC0();
	CFG_Clock_UART();
	cfg_UART0_mode1();
	init_Serial_Buffer();   
	init_Serial_Buffer1();
	CFG_UART1();
	NSS_slave = 1;
	TR2 = 0;
	EA=1;

	
	//Courant_ADC();
	fonctionRoutage(commande);

	serOutstring("\n\rDemarrage robot\n\r>");

// a commenter si le robot est déja allumé avant le lancement du code	 
// Pour recevoir le message de démarrage du serializer
/*	do{
			a=serInchar1();
			echo[0] = a;
			echo[1] = '\0';
			serOutstring(echo);
			memset(echo,0,strlen(echo));
		}while(a!=0x3E);
*/
	while(1){

		memset(message_s,0,sizeof(message_s));
		memset(mess, 0, strlen(mess));		
		memset(message_PC_com, 0, strlen(message_PC_com));		

		commande_correct =1;
		
		i=0;
		a=0;
		memset(com, 0, 50);
		do{
			a=serInchar();
			echo[0] = a;
			echo[1] = '\0';
			serOutstring(echo);
			memset(echo,0,strlen(echo));
			if (a!=0x00)
				{
				com[i]=a;
				i=i+1;
				}
			}while(a!=0x0D);
		

		commande = Message(com);
		if (commande_correct == 1){
				fonctionRoutage(commande);
		}
		if (commande_correct == 2){
			serOutstring(message_PC_com); // si erreur
		}
		if(commande_correct == 0){
			strcpy(mess, "\n\r#\n\r>");
			serOutstring(mess); // si erreur
		}
	//serOutstring(message_PC_com); // si erreur
	}
	
}

void ISR_Timer2 (void) interrupt 5 {
	TF2 = 0; //Remise à '0' du flag d'overflow
	PWM_servo=!PWM_servo; //On envoie le signal PWM au servomoteur
}
