#include <c8051f020.h>

#include "Fonctions_cote_serializer.h"

//#include "FO_M2_Structures_COMMANDES_INFORMATIONS_Serializer.h"

/** 
convertion ordre reçu par le centrale de controle en ordre pour 
le robot
Fonction decodage action

Fonction mouvement elementaire :
avancer (distance)
avancer (temps, vitesse)

reculer (distance)
reculer (temps, vitesse)

tourner (degrée)
**/

/**
All commands and parameters are separated by spaces. 
There are two types of parameters: Simple and Complex. 
Simple parameters are basically simple strings, 
such as “hello” or “1”. 
Complex parameters are simple strings separated by colons “:”, 
such as “hello:world” or “2:1”.message fini par /r

All commands (and associated parameters) are terminated by an ASCII 
carriage-return character “\r” (0x0D).

reponse du serializer : 
Ret/r/n< -> réussite + ret = réponse de retour
ACK/r/n< -> réussite sans réponse de retour
NACK/r/n< -> échec

**/

int conv_distance_ticks(int d){ // convertion d[m] en ticks  
	int ret = ceil(d/(0.3*0.001)); //624ticks = 1tour et diamêtre roue = 60mm
	return ret;
}

int conv_angle_ticks(int angle){ 
	int ret;
	float a = angle/360.0;
	ret = ceil(4600*a); //2346ticks = 1 360° du robot
	return ret;
}

void itos(int i, char * ret){
	char nbr3[3];
	char nbr2[2];
	char nbr1[1];
	int c = i/100;
	int d = (i-c*100)/10;
	int u = i-c*100-d*10;
	
	if (i > 99) {
		nbr3[0] = c+'0';
		nbr3[1] = d+'0';
		nbr3[2] = u+'0';
		strcpy(ret,nbr3);
	}
	else if (i > 9) {
		nbr2[0] = d+'0';
		nbr2[1] = u+'0';
		strcpy(ret,nbr2);
	}
	else {
		nbr1[0] = u+'0';
		strcpy(ret,nbr1);
	}
}

int true_vit(unsigned char vit){
	int ret = vit;
	
	if ( vit >= 100){
		ret = -(vit - 100);
	}
	return ret;
}

void true_ticks(int ticks, char * ret){
	char str[4] = {0};
	int l = 0;

	if ( ticks >= 0){ //  différentitation angle positife et négatif
		sprintf(ret, "%d", ticks);
	}
	else{
		ticks = -1*ticks;
		strcpy(ret,"-");
		sprintf(str, "%d", ticks);
		strcat(ret,str);
		l = strlen(str)+2;
		ret[l]='\0'; // on force la fin de chaine
	}
}
/* entré : une structure représenatnt une commande destiné au serializer
	sortie : une chaine de caractère qui est la commande à envoyer au serializer

fonctionnement : à l'aide de l'enum Etat_commande qui représente l'action à éffectué par le serializer
 on effectue un switch qui construit une chaine de caractère avec les paramêtre necessaire
*/

/*
A faire : astuce pour les ticks négatif : test avant le sprintf si 
le int > 63... et si oui copie  d'un moins avant sinon copie normale
*/
void formate_serializer(struct COMMANDES_SERIALIZER com, char * ret){
	char nbr[9];
	
	switch (com.Etat_Commande){
		case Reset :{
			strcpy(ret,"reset");
			break;
		}
		case Getenc_1 :{
			break;
		}
		case Getenc_2 :{
			break;
		}
		case Getenc_1_2 :{
			break;
		}
		case Clrenc_1 :{
				break;
		}
		case Clrenc_2 :{
			break;
		}
		case Clrenc_1_2 :{
			break;
		}
		case mogo_1 :{
			strcpy(ret,"mogo 1:");
			sprintf(nbr, "%d", true_vit(com.Vitesse_Mot1));
			strcat(ret,nbr);
			break;
		}
		case mogo_2 :{
			strcpy(ret,"mogo 2:");
			sprintf(nbr, "%d", true_vit(com.Vitesse_Mot2));
			strcat(ret,nbr);
			break;
		}
		case mogo_1_2 :{
			strcpy(ret,"mogo 1:");
			sprintf(nbr, "%d", true_vit(com.Vitesse_Mot1));
			strcat(ret,nbr);
			strcat(ret," 2:");
			sprintf(nbr, "%d", true_vit(com.Vitesse_Mot2));
			strcat(ret,nbr);
			break;
		}
		case Vpid_set :{
			strcpy(ret,"vpid ");
			sprintf(nbr, "%d", com.Set_P);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", com.Set_I);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", com.Set_D);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", com.Set_L_A);
			strcat(ret,nbr);
			strcat(ret,":");
			break;
		}
		case Vpid_read :{
			strcpy(ret,"vpid");
			break;
		}
		case digo_1 :{
			strcpy(ret,"digo 1:");
			true_ticks(com.Ticks_mot1, nbr);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", true_vit(com.Vitesse_Mot1));
			strcat(ret,nbr);
			break;
		}
		case digo_2 :{
			strcpy(ret,"digo 2:");
			sprintf(nbr, "%d", true_vit(com.Vitesse_Mot2));
			strcat(ret,nbr);
			strcat(ret,":");
			true_ticks(com.Ticks_mot2, nbr);
			strcat(ret,nbr);
			break;
		}
		case digo_1_2 :{
			strcpy(ret,"digo 1:");
			true_ticks(com.Ticks_mot1, nbr);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", true_vit(com.Vitesse_Mot1));
			strcat(ret,nbr);
			strcat(ret," 2:");
			true_ticks(com.Ticks_mot2, nbr);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", true_vit(com.Vitesse_Mot2));
			strcat(ret,nbr);
			break;
		}
		case Dpid_set :{
			strcpy(ret,"dpid ");
			sprintf(nbr, "%d",com.Set_P);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", com.Set_I);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", com.Set_D);
			strcat(ret,nbr);
			strcat(ret,":");
			sprintf(nbr, "%d", com.Set_L_A);
			strcat(ret,nbr);
			strcat(ret,":");
			break;
		}
		case Dpid_read :{
			strcpy(ret,"vpid");
			break;
		}
		case Rpid_Stinger :{
			break;
		}
		case Pids :{
			break;
		}
		case Stop :{
			strcpy(ret,"stop");
			break;
		}
		case Vel :{
			strcpy(ret,"vel");
			break;
		}
		case Restore :{
			strcpy(ret,"restore");
			break;
		}
		default:
			break;
	}
	strcat(ret,"\r");
}
/* entré : une structure représentant une commande envoyé par le PC de commande
	sortie : une structure représentant une commande destiné 

fonctionnement : à l'aide de l'enum Etat_Epreuve et Etat_Mouvement
on remplit une strucutre COMMANDES_SERIALIZER avec les paramètre destiné au serializer
*/
struct COMMANDES_SERIALIZER transcode_commande_to_serializer (struct COMMANDES com) {
	struct  COMMANDES_SERIALIZER ret;
	char v_angl = 20;
	char v_line = 30;

	switch (com.Etat_Epreuve){//"D" "E" "Q"
		case Stop_Urgence:{
			ret.Etat_Commande = Stop;
			break;
		}
		default:
			break;
	}
		
	switch (com.Etat_Mouvement){
		// avancer	
		case Avancer:{ 
			ret.Etat_Commande = mogo_1_2;
			if (com.Vitesse == '0'){
				com.Vitesse = v_line;
			}
			ret.Vitesse_Mot1 = com.Vitesse;
			ret.Vitesse_Mot2 = com.Vitesse;
			break;
		}
		// reculer
		case(Reculer):{ 
				ret.Etat_Commande = mogo_1_2;
				if (com.Vitesse == '0'){
					com.Vitesse = v_line;
				}
			  ret.Vitesse_Mot1 = com.Vitesse + 100;
			  ret.Vitesse_Mot2 = com.Vitesse + 100;
				break;
		}
		// stop		
		case(Stopper):{ 
				ret.Etat_Commande = Stop;
				break;
		}
		// rotation 90° droite 
		case(Rot_90D):{ // motor1 = droite motor2 = gauche
				if (com.Vitesse == '0'){
					com.Vitesse = v_angl;
				}
				ret.Etat_Commande = digo_1_2;
			  ret.Vitesse_Mot1 = com.Vitesse;
			  ret.Ticks_mot1 = -conv_angle_ticks(45);
			  ret.Vitesse_Mot2 = (com.Vitesse);
				ret.Ticks_mot2 = conv_angle_ticks(45);
				break;
		}
		// rotation 90° gauche
		case(Rot_90G):{ 
				if (com.Vitesse == '0'){
					com.Vitesse = v_angl;
				}
				ret.Etat_Commande = digo_1_2;
			  ret.Vitesse_Mot1 = (com.Vitesse);
			  ret.Ticks_mot1 = conv_angle_ticks(45);
			  ret.Vitesse_Mot2 = com.Vitesse;
				ret.Ticks_mot2 = -conv_angle_ticks(45);
				break;
		}
		// rotation 180° droite
		case(Rot_180D):{ 
				if (com.Vitesse == '0'){
					com.Vitesse = v_angl;
				}
				ret.Etat_Commande = digo_1_2;
			  ret.Vitesse_Mot1 = com.Vitesse;
			  ret.Ticks_mot1 = -conv_angle_ticks(90);
			  ret.Vitesse_Mot2 = (com.Vitesse);
				ret.Ticks_mot2 = conv_angle_ticks(90);
				break;
		}
		// rotation 180° gauche
		case(Rot_180G):{ 
				if (com.Vitesse == '0'){
					com.Vitesse = v_angl;
				}
				ret.Etat_Commande = digo_1_2;
			  ret.Vitesse_Mot1 = (com.Vitesse);
			  ret.Ticks_mot1 = conv_angle_ticks(90);
			  ret.Vitesse_Mot2 = com.Vitesse;
				ret.Ticks_mot2 = -conv_angle_ticks(90);
				break;
		}
				
		// rotation d'un angle donné à droite
		case(Rot_AngD):{ 
				if (com.Vitesse == '0'){
					com.Vitesse = v_angl;
				}
				ret.Etat_Commande = digo_1_2;
			  ret.Vitesse_Mot1 = com.Vitesse;
			  ret.Ticks_mot1 = -conv_angle_ticks(com.Angle/2);
			  ret.Vitesse_Mot2 = (com.Vitesse);
				ret.Ticks_mot2 = conv_angle_ticks(com.Angle/2);
				break;
		}
		// rotation d'un angle donné à droite		
		case(Rot_AngG):{ 
				if (com.Vitesse == '0'){
					com.Vitesse = v_angl;
				}
				ret.Etat_Commande = digo_1_2;
			  ret.Vitesse_Mot1 = (com.Vitesse);
			  ret.Ticks_mot1 = conv_angle_ticks(com.Angle/2);
			  ret.Vitesse_Mot2 = com.Vitesse;
				ret.Ticks_mot2 = -conv_angle_ticks(com.Angle/2);
				break;
		}
				
		case(Depl_Coord):{
				if (com.Vitesse == '0'){
					com.Vitesse = v_angl;
				}
				if (com.Angle >= 0 ){
					ret.Etat_Commande = digo_1_2;
					ret.Vitesse_Mot1 = (com.Vitesse);
					ret.Ticks_mot1 = conv_angle_ticks(com.Angle/2);
					ret.Vitesse_Mot2 = com.Vitesse + 100;
					ret.Ticks_mot2 = -conv_angle_ticks(com.Angle/2);
				}
				else {
					com.Angle = -com.Angle;
					ret.Etat_Commande = digo_1_2;
					ret.Vitesse_Mot1 = com.Vitesse + 100;
					ret.Ticks_mot1 = -conv_angle_ticks(com.Angle/2);
					ret.Vitesse_Mot2 = (com.Vitesse);
					ret.Ticks_mot2 = conv_angle_ticks(com.Angle/2);
				}
				break;
		}
		default:
				break;
	}

	return ret;
}
	
	
