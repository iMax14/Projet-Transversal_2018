#include <c8051f020.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "ultrason.h"
#include "Detect_Obst.h"
#include "Servomoteur_Horizontal.h"

struct INFORMATIONS information;
float detection_avant;
float detection_arriere;
char obstacle_avant[5];
char obstacle_arriere[5];
char affichage [256] = 0;
extern signed int Angle_atteint; // Initaliser � "XX"
char Angle_servo [1] = 0;

char detection_AV(struct COMMANDES com){


  detection_avant=MES_Dist_AV();
  if (detection_avant != 0){ //Si un obstacle est d�tect�
    information.Nbre_Val_obst ++; //On incr�mente le nbr d'abstacle d�tect�
  }
  if(com.A_Obst != Obst_proche_balayage){
    //Affichage
    sprintf(obstacle_avant,"%f",detection_avant); // Convertion de float � char
		sprintf(Angle_servo,"%si",Angle_atteint); // Convertion de float � char

    strcat(affichage,Angle_servo); //On donne la position du Servomoteur_Horizontal
    strcat(affichage," : AV -> ");
    strcat(affichage,obstacle_avant); //On donne la distance � laquelle est l'obstacle AV
  }

  return detection_avant;
}


char detection_AV_AR(struct COMMANDES com){
  detection_avant=MES_Dist_AV();
  detection_arriere=MES_Dist_AR();
  if (detection_avant != 0){ //Si un obstacle est d�tect�
    information.Nbre_Val_obst++; //On incr�mente le nbr d'abstacle d�tect�
  }
  if (detection_arriere != 0){ //Si un obstacle est d�tect�
    information.Nbre_Val_obst++; //On incr�mente le nbr d'abstacle d�tect�
  }
  if(com.A_Obst != Obst_proche_balayage){
    //Affichage

    sprintf(obstacle_avant,"%f",detection_avant); // Convertion de float � char
    sprintf(obstacle_arriere,"%f",detection_arriere); // Convertion de float � char
		sprintf(Angle_servo,"%si",Angle_atteint); // Convertion de float � char

    strcat(affichage,Angle_servo); //On donne la position du Servomoteur_Horizontal
    strcat(affichage," : AV -> ");
    strcat(affichage,obstacle_avant); //On donne la distance � laquelle est l'obstacle AV
    strcat(affichage," : AR -> ");
    strcat(affichage,obstacle_arriere); //On donne la distance � laquelle est l'obstacle AR
  }

  return detection_avant,detection_arriere;
}


struct INFORMATIONS Detect_Obst(struct COMMANDES com){
	signed int i;
	int j;
	int k=0;
	extern float detection_avant,detection_arriere;
	float tab_dist_AV [50];
	float tab_dist_AR [50];
	signed int tab_angles [50];
	float dist_min_AV [5];
	float dist_min_AR [5];
	float dist_min [5];
	char dist_min_ascii [5];
	signed int angle [5];
	char angle_ascii [5];
	int min = 0;
	int min_AV = 0;
	int min_AR = 0;

	switch(com.A_Obst){
		case Obst_unique:
			if (com.Det==avant){ //Calcul uniquement pour le t�l�m�tre AV
				detection_AV(com);
			}
			else{ //Calcul pour les t�l�m�tres AV & AR
				detection_AV_AR(com);
			}
			break;

		case Obst_balayage:
			if (com.Etat_DCT_Obst==oui_180){ //detection AV sur 180�
				for(i = -90;i<=90;i = i+com.DCT_Obst_Resolution){
					CDE_Servo_H(i); // On met le servomoteur en position
					detection_AV(com); //On fait une d�tection AV
				}
			}
			else{ //detection AV et AR sur 360�
				for(i = -90;i<=90;i = i+com.DCT_Obst_Resolution){
					CDE_Servo_H(i); // On met le servomoteur en position
					detection_AV_AR(com); //On fait une d�tection AV et AR
				}
			}
			break;


// A TESTER AVEC LE SIMULATEUR
		case Obst_proche_balayage:
			if (com.Etat_DCT_Obst==oui_180){ //detection AV sur 180�
				for(i = -90;i<=90;i = i+com.DCT_Obst_Resolution){
					tab_angles[k] = CDE_Servo_H(i); // On met le servomoteur en position
					tab_dist_AV[k] = detection_AV(com); //On fait une d�tection AV
					k++;
				}
				for (j=0; j<= (180/com.DCT_Obst_Resolution); j++){//Recherche de la distance minimale parmis toutes celle trouv�e
		 			if(tab_dist_AV[j] < tab_dist_AV[min]){
						min = j;
					}
				}
				dist_min[0] = tab_dist_AV[min];
				angle[0] = tab_angles[min];

				//Affichage
				sprintf(dist_min_ascii,"%f",dist_min[0]); // Convertion de float � char
				sprintf(angle_ascii,"%d",angle[0]); // Convertion de int � char

				strcat(affichage,angle_ascii); //On donne la position du Servomoteur_Horizontal
				strcat(affichage," : AV -> ");
				strcat(affichage,dist_min_ascii); //On donne la distance � laquelle est l'obstacle AV
			}
			else{ //detection AV et AR sur 360�
				for(i = -90;i<=90;i = i+com.DCT_Obst_Resolution){
					tab_angles[k] = CDE_Servo_H(i); // On met le servomoteur en position
					tab_dist_AV[k],tab_dist_AR[k] = detection_AV_AR(com); //On fait une d�tection AV et AR
					k++;
				}
				for (j=0; j<= (180/com.DCT_Obst_Resolution); j++){//Recherche de la distance minimale AV parmis toutes celle trouv�e
					if(tab_dist_AV[j] < tab_dist_AV[min_AV]){
						min_AV = j;
					}
				}
				dist_min_AV[0] = tab_dist_AV[min_AV];
				for (j=0; j<= (180/com.DCT_Obst_Resolution); j++){//Recherche de la distance minimale AR parmis toutes celle trouv�e
					if(tab_dist_AR[j] < tab_dist_AR[min_AR]){
						min_AR = j;
					}
				}
				dist_min_AR[0] = tab_dist_AR[min];
				if (dist_min_AR < dist_min_AV){
					dist_min[0] = dist_min_AR[min_AR];
					angle[0] = tab_angles[min_AR];
				}
				else{
					dist_min[0] = dist_min_AV[min_AV];
					angle[0] = tab_angles[min_AV];
				}

				//Affichage
				sprintf(dist_min_ascii,"%f",dist_min[0]); // Convertion de float � char
				sprintf(angle_ascii,"%d",angle[0]); // Convertion de int � char

				strcat(affichage,angle_ascii); //On donne la position du Servomoteur_Horizontal
				strcat(affichage," : ");
				strcat(affichage,dist_min_ascii); //On donne la distance � laquelle est l'obstacle AV
			}
			break;
	}

	return information;
}
