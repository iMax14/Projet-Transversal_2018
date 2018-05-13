#include <c8051f020.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "ultrason.h"
#include "encodage_son.h"
#include "Servomoteur_Horizontal.h"

struct INFORMATIONS encode_son(struct COMMANDES com){
	struct INFORMATIONS information;
	float detection_avant=50.0f;
	float detection_arriere=40.0f;
	char obstacle_avant[15];
	char obstacle_arriere[15];
	char tab[50];
	char *debut_donnee=" XX:";
	int n;
	int i;
	char angle[4];
	char angle1[4];
	char angle2[4];
	char val;
	char val1;
	char val2;
	char val_proche;


	information.Tab_Val_Obst=tab;
  information.Nbre_Val_obst=0;
	switch(com.A_Obst){
		case Obst_unique:
			if (com.Det==avant){
				// detection seulement a l'avant  mesure et envoie de la donnée
				
				detection_avant=MES_Dist_AV();
				if( detection_avant!=0){
					sprintf( obstacle_avant,"%f",detection_avant);
					strcpy(tab,debut_donnee);
					strcat(tab,obstacle_avant);
					strcat(tab,"\0");
					information.Nbre_Val_obst=1;
				}
				else{
					strcpy(tab,"Pas d'obstacle\n\r>");
				}
			}
			else{				
				// mesure avant et arriere à la position XX du servomoteur (non connu)
				detection_avant=MES_Dist_AV();
				detection_arriere=MES_Dist_AR();
				if( detection_avant!=0){
					sprintf( obstacle_avant,"%f",detection_avant);
					strcpy(tab,debut_donnee);
					strcat(tab,obstacle_avant);
					information.Nbre_Val_obst++;
				}
				if( detection_arriere!=0){
					sprintf( obstacle_arriere,"%f",detection_arriere);
					if(information.Nbre_Val_obst!=0){
						strcat(tab,debut_donnee);
						strcat(tab,obstacle_arriere);
					}
					else{	
						strcpy(tab,debut_donnee);
						strcat(tab,obstacle_arriere);
					}
					information.Nbre_Val_obst++;
				}
			}
			break;
		case Obst_balayage:
			//detection 180
			if (com.Etat_DCT_Obst==oui_180){
				//mesure initiale
				n=180/com.DCT_Obst_Resolution;	
				CDE_Servo_H(-90);
				detection_avant=MES_Dist_AV();
				strcpy(tab," ");
				if ( detection_avant!=0){
					sprintf( obstacle_avant,"%f",detection_avant);
					strcat(tab,"-90:");
					strcat(tab,obstacle_avant);			
					information.Nbre_Val_obst++;
				}
				// autres mesures
				for(i=1; i<n; i++){
					val=-90 +i*com.DCT_Obst_Resolution;
					sprintf(angle,"%c",val);
					angle[3]=':';
					CDE_Servo_H(val);
					detection_avant=MES_Dist_AV();
					if ( detection_avant!=0){		
						sprintf( obstacle_avant,"%f",detection_avant);
						strcat(tab,angle);			
						strcat(tab,obstacle_avant);	
						information.Nbre_Val_obst++;	
					}						
				}
			}
			//detection 360 ( avant et arriere)
			else{
				//mesure initiale 
				n=360/com.DCT_Obst_Resolution;
				CDE_Servo_H(-90);
				strcpy(tab," ");
				detection_avant=MES_Dist_AV();
				if (detection_avant!=0){
					sprintf( obstacle_avant,"%f",detection_avant);
					strcat(tab,"-90:");
					strcat(tab,obstacle_avant);
					information.Nbre_Val_obst++;
				}
				detection_arriere=MES_Dist_AR();
				if (detection_arriere!=0){
					sprintf( obstacle_arriere,"%f",detection_arriere);
					strcat(tab,"90:");
					strcat(tab,obstacle_arriere);
					information.Nbre_Val_obst++;
				}
				//autres mesures
				for(i=0; i<n; i++){
					//les angles mesurés
					val1=-90 +i*com.DCT_Obst_Resolution;
					if (val1>0){
						val2=(-180 +i*com.DCT_Obst_Resolution);
					}
					else{
						val2=90 +i*com.DCT_Obst_Resolution;
					}
					CDE_Servo_H(-90 +i*com.DCT_Obst_Resolution);
					detection_avant=MES_Dist_AV();
					if (detection_avant!=0){
						sprintf(angle1,"%c",val1);
						angle1[3]=':';
						sprintf( obstacle_avant,"%f",detection_avant);
						strcat(tab,angle1);			
						strcat(tab,obstacle_avant);	
						information.Nbre_Val_obst++;
					}
					detection_arriere=MES_Dist_AR();
					if( detection_arriere!=0){
						sprintf(angle2,"%c",val2);
						angle2[3]=':';
						sprintf( obstacle_arriere,"%f",detection_arriere);
						strcat(tab,angle2);			
						strcat(tab,obstacle_arriere);
						information.Nbre_Val_obst++;						
					}
				}
			}
			break;
		case Obst_proche:
		//cas detection sur 180 degré
			if (com.Etat_DCT_Obst==oui_180){
				//initialisation
				n=180/com.DCT_Obst_Resolution;	
				CDE_Servo_H(-90);
				detection_avant=MES_Dist_AV();
				if(detection_avant!=0){
					val_proche=detection_avant;
					sprintf( obstacle_avant,"%f",detection_avant);
					strcpy(tab,"-90:");
					strcat(tab,obstacle_avant);
				}
				for ( i=1; i<n; i++){
					val=-90 +i*com.DCT_Obst_Resolution;
					sprintf(angle,"%c",val);
					angle[3]=':';
					CDE_Servo_H(val);
					detection_avant=MES_Dist_AV();
					if(detection_avant<val_proche && detection_avant!=0){
						sprintf( obstacle_avant,"%f",detection_avant);
						strcpy(tab,angle);
						strcat(tab,obstacle_avant);			
						val_proche=detection_avant;
					}				
				}
			}
			//cas detection 360
			else{
				//initialisation
				n=360/com.DCT_Obst_Resolution;
				CDE_Servo_H(-90);
				detection_avant=MES_Dist_AV();
				detection_arriere=MES_Dist_AR();
				if (detection_arriere<detection_avant && detection_arriere!=0){
					sprintf( obstacle_arriere,"%f",detection_arriere);
					strcpy(tab,"90:");
					strcat(tab,obstacle_arriere);
					val_proche=detection_arriere;
				}
				else if (detection_avant!=0){
					sprintf( obstacle_avant,"%f",detection_avant);
					strcpy(tab,"-90:");
					strcat(tab,obstacle_avant);
					val_proche=detection_avant;
				}
				for ( i=1; i<n; i++){
					//les angles mesurés
					val1=-90 +i*com.DCT_Obst_Resolution;
					val2=-90 +i*com.DCT_Obst_Resolution+180;
					//deplacement du servomoteur
					CDE_Servo_H(val1);
					detection_avant=MES_Dist_AV();
					detection_arriere=MES_Dist_AR();
					if (detection_avant<detection_arriere && detection_avant!=0){
						if (detection_avant<val_proche){
							sprintf(angle1,"%c",val1);
							angle1[3]=':';
							sprintf( obstacle_avant,"%f",detection_avant);
							strcpy(tab,angle1);
							strcat(tab,obstacle_avant);
							val_proche=detection_avant;
						}
					}
					else if ( detection_arriere !=0){
						if (detection_arriere<val_proche){
							sprintf(angle2,"%c",val2);
							angle2[3]=':';
							sprintf( obstacle_arriere,"%f",detection_arriere);
							strcpy(tab,angle2);
							strcat(tab,obstacle_arriere);
							val_proche=detection_arriere;
						}
					}
				}
				
			}
			break;
	}	
	return information;
}
