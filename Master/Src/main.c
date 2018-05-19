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
#include "Config_ADC.h"
#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "UART0_RingBuffer_lib.h"
#include "ConfigUART1.h"
#include "UART0_RingBuffer_lib.h"
#include "Fonctions_cote_serializer.h"
#include "Lien_Structure_commande.h"
#include "Mesure_courant.h"

#ifndef CFG_Globale
  #define CFG_Globale
  #include "CFG_Globale.h"
#endif


//------------------------------------------------------------------------------------
// D�clarations des variables globales
//------------------------------------------------------------------------------------
int vitesse_par_defaut = 10;
char message_PC_com[50] = {0};
enum Epreuve epreuve_en_cours = Epreuve_non;
int commande_correct = 0;
double energie = 0;

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

signed int conv(char* nbr, int taille)
{
	int k=0;
	double t1 = 0;
	signed int ret=0;
	int numero=0;

	if(nbr[0]=='-')
	{
		for(k=1;k<taille;k++)
		{
			numero=nbr[k]-'0';
			t1 = pow(10,taille-1-k);
			numero = numero*ceil(pow(10,taille-1-k));
			ret+=numero;
		}
		ret=-1*ret;
	}
	else
	{
		for(k=0;k<taille;k++)
		{
			numero=(nbr[k]-'0')*ceil(pow(10,taille-1-k));
			ret+=numero;
		}
	}
	return ret;
}
signed int conversionangle_positif (unsigned char tableau[3]){
	signed int centaine=0;
	signed int dizaine=0;
	signed int unite=0;
	signed int valeur;

	if (tableau[0] == '-') {
		dizaine= (tableau[1]-'0')*10;
		unite= (tableau[2]-'0');
		valeur = -1* (dizaine + unite);
	}
	else {
		centaine= (tableau[0]-'0')*100;
		dizaine= (tableau[1]-'0')*10;
		unite= (tableau[2]-'0');
		valeur= centaine+dizaine+unite;
	}
	return valeur	;
}

struct COMMANDES traitement_A(char * com, struct COMMANDES commande){ // traitement lorsque la premi�re lettre est A (AVANCER A DUREE DAQUISITION) AVANCER OK
	char vitesse;
	char tab[3];
	int j;
	char duree;

	switch (com[1]) //AJOUTER LA DUREE
			{
				case 'S': // cas ASS (Dur�e)
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
						// on regarde le param�tre de la vitesse
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
				case '1': //�preuve 1
				{
					commande.Etat_Epreuve=epreuve1;
					strcpy(message_PC_com, "\r\nInvite de commande 1\r\n>");
					epreuve_en_cours = epreuve1;
					//serOutstring("Invite de commande 1");
					break;

				}
				case '2': //�preuve 2
				{
					commande.Etat_Epreuve=epreuve2;
					strcpy(message_PC_com, "\r\nInvite de commande 2\r\n>");
					epreuve_en_cours = epreuve2;
					//serOutstring("Invite de commande 2");
					break;

				}
				case '3': //�preuve 3
				{
					commande.Etat_Epreuve=epreuve3;
					epreuve_en_cours = epreuve3;
					strcpy(message_PC_com, "\r\nInvite de commande 3\r\n>");
					//serOutstring("Invite de commande 3");
					break;

				}
				case '4': //�preuve 4
				{
					commande.Etat_Epreuve=epreuve4;
					epreuve_en_cours = epreuve4;
					strcpy(message_PC_com, "\r\nInvite de commande 4\r\n>");
					//serOutstring("Invite de commande 4");
					break;

				}
				case '5': //�preuve 5
				{
					commande.Etat_Epreuve=epreuve5;
					epreuve_en_cours = epreuve5;
					strcpy(message_PC_com, "\r\nInvite de commande 5\r\n>");
					//serOutstring("Invite de commande 5");
					break;
				}
				case '6': //�preuve 6
				{
					commande.Etat_Epreuve=epreuve6;
					epreuve_en_cours = epreuve6;
					strcpy(message_PC_com, "\r\nInvite de commande 6\r\n>");
					//serOutstring("Invite de commande 6");
					break;

				}
				case '7': //�preuve 7
				{
					commande.Etat_Epreuve=epreuve7;
					epreuve_en_cours = epreuve7;
					strcpy(message_PC_com, "\r\nInvite de commande 7\r\n>");
					//serOutstring("Invite de commande 7");
					break;
				}
				case '8': //�preuve 8
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
	//serOutstring("Fin de l'�preuve");
	commande.Etat_Epreuve=Epreuve_non;
	commande.Etat_Mouvement =Mouvement_non;
	epreuve_en_cours = Epreuve_non;
	return commande;
}
struct COMMANDES traitement_G(char * com, struct COMMANDES commande) // DEPLACEMENT AVEC ANGLE : OK
{
		int i=4;
		int k=0;
		char coord[10];
		if (epreuve_en_cours == epreuve1 && com[1]==' ' && com[2]=='X' && com[3]==':')
		{
			commande.Etat_Mouvement = Depl_Coord;

			while(com[i]!= ' ')
			{
				coord[k]=com[i];
				i=i+1;
				k++;
			}
			coord[k]='\0';
			commande.Coord_X=conv(coord,strlen(coord));
			i = i+ 3;
			k=0;
			while(com[i]!= ' ')
			{
				coord[k]=com[i];
				i=i+1;
				k=k+1;
			}
			coord[i]='\0';
			commande.Coord_Y=conv(coord,strlen(coord));
			k=0;
			i = i+ 3;
			while(com[i]!= '\0')
			{
				coord[k]=com[i];
				i=i+1;
				k=k+1;
			}
			coord[i-1]='\0';
			commande.Angle=conv(coord,strlen(coord));


		}
		else
		{
			commande_correct=0;
		}


			return commande;

}
struct COMMANDES traitement_I(char * com, struct COMMANDES commande) // pas encore utilis�
{
	// a compl�ter
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
				case 'E': //ME Mesure de l'�nergie
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
									commande.Etat_DCT_Obst=oui_360; // d�fault de D
									for (j=6; j<8;j++)
									{
										tab[j-6]=com[j];
									}
									angle=conversioncoord(tab);
									if (angle%5==0){
										if(angle>=5 && angle<=45){
											commande.DCT_Obst_Resolution=angle;
											commande_correct=1;
										}
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
										if (angle%5==0){
											if(angle>=5 && angle<=45){
												commande.DCT_Obst_Resolution=angle;
												commande_correct=1;
											}
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
						case 'S': // cas MOS
							commande.A_Obst=Obst_proche_balayage;
							switch(com[4])
							{
								case 'A':
									commande.Etat_DCT_Obst=oui_360; // d�fault de D
									for (j=6; j<8;j++)
									{
										tab[j-6]=com[j];
									}
									angle=conversioncoord(tab);
									if (angle%5==0){
										if(angle>=5 && angle<=45){
											commande.DCT_Obst_Resolution=angle;
											commande_correct=1;
										}
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
										if (angle%5==0){
											if(angle>=5 && angle<=45){
												commande.DCT_Obst_Resolution=angle;
												commande_correct=1;
											}
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
	// a compl�ter
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
struct COMMANDES traitement_S(char * com, struct COMMANDES commande/*,char f_b, char t_son, char t_silence, char bip_b*/) // STOPPER
{
	int i;
	char f_b=6;
	char t_son=25;
	char t_silence=50;
	char bip_b=3;
	char tab[2];
	char frequence;
	char duree_son;
	char duree_silence;
	char nombre_Bips;

	if( com[1]=='D')
	{
		commande.son=emission;
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
				commande.frequence=f_b;								// d�finit par le code fr�quence allant de 1 a 99 (mais defini de 1 a 21)
				commande.duree_son=t_son;									// dur�e du signal sonore
				commande.duree_silence=t_silence;							// duree du silence
				commande.nombre_Bips=bip_b;							// nombre de bips
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
	// on regarde le param�tre de la vitesse

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

struct COMMANDES Message (char * com/*, char f_b,char t_son,char t_silence,char bip_b*/){
//	char tabcoordx[2];
//	char tabcoordy[2];
//	char tabangle[2];
//	signed char coordx;
//	signed char coordy;
//	char angle;
//	int cpt=0;
	//a modifier



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
	commande.son=non_emission;

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
		case 'D':// si on recoit un D (D�but de l'epreuve)
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
			commande=traitement_S(com, commande/*,f_b, t_son, t_silence, bip_b*/);

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
//D�claration des variables
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

	//a modifier
	char f_b=6;
	char t_son=25;
	char t_silence=50;
	char bip_b=3;

	EA=0;
	Init_Device();  // Initialisation du microcontr�leur
	Config_Timer2();
	Config_timer0();
	Config_Timer3();
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
  energie = 0;
	EA=1;


	//Courant_ADC();
	//fonctionRoutage(commande);

	serOutstring("\n\rDemarrage robot\n\r>");
// a commenter si le robot est d�ja allum� avant le lancement du code
// Pour recevoir le message de d�marrage du serializer
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
		/*do{
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
			}while(a!=0x0D); Commenté pour les tests avec le simulateur*/

		//strcpy(com,"CS V A:-90");
		//strcpy(com,"SD F:12 P:50 W:60 B:5");
		//strcpy(com,"MOB");

		commande = Message(com/*, f_b, t_son, t_silence, bip_b*/);
			/*if(commande.son==emission)
			{
				f_b=commande.frequence;
				t_son=commande.duree_son;
				t_silence=commande.duree_silence;
				bip_b=commande.nombre_Bips;
			}*/
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
	TF2 = 0; //Remise � '0' du flag d'overflow
	PWM_servo=!PWM_servo; //On envoie le signal PWM au servomoteur
}

void ISR_Timer3 (void) interrupt 14 {
	TMR3CN &= 0x04; //Remise � '0' du flag d'overflow
	energie += 9.6*Courant_ADC()*0.001*0.035;
}
