#include <c8051f020.h>
#include <intrins.h>
#include <string.h>
#include <stdio.h>

#include "Servomoteur_Vertical.h"
#include "Transmission_SPI.h"
#include "Transmission_UART0.h"
#include "Declarations_GPIO.h"


#define SYSCLK 22118400L

int cpt1_CM4 = 0;
int cpt1_PointLum = 0;
int cpt1_ServVert = 0;
int cpt1_Vue = 0;
int cpt1_FPGA = 0;
int i = 0;
int cpt_ok = 0;
int test_perte = 0;
int k;

int cpt2_CM4 = 2;
int cpt2_PointLum = 2;
int cpt2_ServVert = 2;
int cpt2_Vue = 2;
int cpt2_FPGA = 2;

// Message recus via la liaison SPI
unsigned char msg_CM4[256] = 0;
unsigned char msg_PointLum[256] = 0;
unsigned char msg_ServVert[256] = 0;
unsigned char msg_Vue[256] = 0;
unsigned char msg_FPGA[256] = 0;

//Variable globale
extern int Dest_msg_SPI[5];


//Fonction d'émission de la connexion SPI
void trame_emise(unsigned char trame_S2M){
	
	SPI0DAT = trame_S2M; // données envoyées
}



//Fonction de réception de la connexion SPI :
void trame_recue(void){

// On regarde quelle trame de début on a reçue pour savoir à qui l'envoyer 
// De plus, on vérifie bien si on en reçoie 2
// De plus, on a ---> 0xD1 : Cortex M4 ; 0xD2 : Pointeur lumineux ; 0xD3 : Servomoteur vertical ; 0xD4 : Prise de vue ; 0xD5 : FPGA

		if (SPI0DAT == 0xD1){
			if(i>0){ //RAZ en cas de déconnection de la trame N-1
				cpt2_CM4 = 2;
				i=0;
			}
			cpt1_CM4++;
			if (cpt1_CM4 == 2){
				cpt1_CM4 = 0;
				cpt2_CM4 = 0;
			}
			return;
		}
		else if (SPI0DAT == 0xD2){
			if(i>0){//RAZ en cas de déconnection de la trame N-1
				cpt2_PointLum = 2;
				i=0;
			}
			cpt1_PointLum++;
			if (cpt1_PointLum == 2){
				cpt1_PointLum = 0;
				cpt2_PointLum = 0;
			}
			return;
		}
		else if (SPI0DAT == 0xD3){
			if(i>0){//RAZ en cas de déconnection de la trame N-1
				cpt2_ServVert = 2;
				i=0;
			}
			cpt1_ServVert++;
			if (cpt1_ServVert == 2){
				cpt1_ServVert = 0;
				cpt2_ServVert = 0;
			}
			return;
		}
		else if (SPI0DAT == 0xD4){
			if(i>0){//RAZ en cas de déconnection de la trame N-1
				cpt2_Vue = 2;
				i=0;
			}
			cpt1_Vue++;
			if (cpt1_Vue == 2){
				cpt1_Vue = 0;
				cpt2_Vue = 0;
			}
			return;
		}
		else if (SPI0DAT== 0xD5){
			if(i>0){//RAZ en cas de déconnection de la trame N-1
				cpt2_FPGA = 2;
				i=0;
			}
			cpt1_FPGA++;
			if (cpt1_FPGA == 2){
				cpt1_FPGA = 0;
				cpt2_FPGA = 0;
			}
			return;
		}
		
		if (SPI0DAT == 0xFF){
// On réinitialise l'indice du message reçue, pour le message suivant
			cpt1_CM4 = 0;
			cpt1_PointLum = 0;
			cpt1_ServVert = 0;
			cpt1_Vue = 0;
			cpt1_FPGA = 0;
// On réinitialise l'indice qui permet d'écrire dans le message 
			i = 0;
			test_perte++;
			// On incrémente le compte suivant pour savoir si les deux trames de fins ont été transmises
			cpt_ok ++;
			if (test_perte == 3){//RAZ en cas de déconnection de la trame N-1
				cpt_ok =2;
			}
		}
//Pour les 5 tests suivants, on recopie le message si et seulement si les deux trames de début correpondant aux messages ont été transmises
		else if (cpt2_CM4 == 0){
			msg_CM4[i] = SPI0DAT;
			i++;
		}
		else if (cpt2_PointLum == 0){
			msg_PointLum[i] = SPI0DAT;
			i++;
		}
		else if (cpt2_ServVert == 0){
			msg_ServVert[i] = SPI0DAT;
			i++;
		}
		else if (cpt2_Vue == 0){
			msg_Vue[i] = SPI0DAT;
			i++;
		}
		else if (cpt2_FPGA == 0){
			msg_FPGA[i] = SPI0DAT;
			i++;
		}
		
// Pour les 5 tests suivants, on appelle la fonction liée au message reçue si et seulement si 
// les deux trames de fin ont été reçues
		
		if (cpt_ok == 2 && cpt2_CM4 == 0){
			Dest_msg_SPI[0] = 1;	
			cpt_ok = 0; //RAZ
			cpt2_CM4 = 2;
		}
		else if (cpt_ok ==2 && cpt2_PointLum == 0){
			Dest_msg_SPI[1] = 1;	
			cpt_ok = 0; //RAZ
			cpt2_PointLum = 2;
		}
		else if (cpt_ok == 2 && cpt2_ServVert == 0){
			Dest_msg_SPI[2] = 1;	
			cpt_ok = 0; //RAZ
			cpt2_ServVert = 2;
		}
		else if (cpt_ok == 2 && cpt2_Vue == 0){
			Dest_msg_SPI[3] = 1;
			cpt_ok = 0; //RAZ
			cpt2_Vue = 2;
		}
		else if (cpt_ok == 2 && cpt2_FPGA == 0){
			Dest_msg_SPI[4] = 1;
			cpt_ok = 0; //RAZ
			cpt2_FPGA = 2;
		}

			
}




/*Fonction de test de la réception de la connexion SPI :
void trame_recue_test(unsigned char s){

// On regarde quelle trame de début on a reçue pour savoir à qui l'envoyer 
// De plus, on vérifie bien si on en reçoie 2
// De plus, on a ---> 0xD1 : Cortex M4 ; 0xD2 : Pointeur lumineux ; 0xD3 : Servomoteur vertical ; 0xD4 : Prise de vue ; 0xD5 : FPGA

		if (s == 0xD1){
			if(i>0){ //RAZ en cas de déconnection de la trame N-1
				cpt2_CM4 = 2;
				i=0;
			}
			cpt1_CM4++;
			if (cpt1_CM4 == 2){
				cpt1_CM4 = 0;
				cpt2_CM4 = 0;
			}
			return;
		}
		else if (s == 0xD2){
			if(i>0){//RAZ en cas de déconnection de la trame N-1
				cpt2_PointLum = 2;
				i=0;
			}
			cpt1_PointLum++;
			if (cpt1_PointLum == 2){
				cpt1_PointLum = 0;
				cpt2_PointLum = 0;
			}
			return;
		}
		else if (s == 0xD3){
			if(i>0){//RAZ en cas de déconnection de la trame N-1
				cpt2_ServVert = 2;
				i=0;
			}
			cpt1_ServVert++;
			if (cpt1_ServVert == 2){
				cpt1_ServVert = 0;
				cpt2_ServVert = 0;
			}
			return;
		}
		else if (s == 0xD4){
			if(i>0){//RAZ en cas de déconnection de la trame N-1
				cpt2_Vue = 2;
				i=0;
			}
			cpt1_Vue++;
			if (cpt1_Vue == 2){
				cpt1_Vue = 0;
				cpt2_Vue = 0;
			}
			return;
		}
		else if (s == 0xD5){
			if(i>0){//RAZ en cas de déconnection de la trame N-1
				cpt2_FPGA = 2;
				i=0;
			}
			cpt1_FPGA++;
			if (cpt1_FPGA == 2){
				cpt1_FPGA = 0;
				cpt2_FPGA = 0;
			}
			return;
		}
		
		if (s == 0xFF){
// On réinitialise l'indice du message reçue, pour le message suivant
			cpt1_CM4 = 0;
			cpt1_PointLum = 0;
			cpt1_ServVert = 0;
			cpt1_Vue = 0;
			cpt1_FPGA = 0;
// On réinitialise l'indice qui permet d'écrire dans le message 
			i = 0;
			test_perte++;
			// On incrémente le compte suivant pour savoir si les deux trames de fins ont été transmises
			cpt_ok ++;
			if (test_perte == 3){//RAZ en cas de déconnection de la trame N-1
				cpt_ok =2;
			}
		}
//Pour les 5 tests suivants, on recopie le message si et seulement si les deux trames de début correpondant aux messages ont été transmises
		else if (cpt2_CM4 == 0){
			msg_CM4[i] = s;
			i++;
		}
		else if (cpt2_PointLum == 0){
			msg_PointLum[i] = s;
			i++;
		}
		else if (cpt2_ServVert == 0){
			msg_ServVert[i] = s;
			i++;
		}
		else if (cpt2_Vue == 0){
			msg_Vue[i] = s;
			i++;
		}
		else if (cpt2_FPGA == 0){
			msg_FPGA[i] = s;
			i++;
		}
		
// Pour les 5 tests suivants, on appelle la fonction liée au message reçue si et seulement si 
// les deux trames de fin ont été reçues
		
		if (cpt_ok == 2 && cpt2_CM4 == 0){
			Dest_msg_SPI[0] = 1;	
			cpt_ok = 0; //RAZ
			cpt2_CM4 = 2;
		}
		else if (cpt_ok ==2 && cpt2_PointLum == 0){
			Dest_msg_SPI[1] = 1;	
			cpt_ok = 0; //RAZ
			cpt2_PointLum = 2;
		}
		else if (cpt_ok == 2 && cpt2_ServVert == 0){
			Dest_msg_SPI[2] = 1;	
			cpt_ok = 0; //RAZ
			cpt2_ServVert = 2;
		}
		else if (cpt_ok == 2 && cpt2_Vue == 0){
			Dest_msg_SPI[3] = 1;
			cpt_ok = 0; //RAZ
			cpt2_Vue = 2;
		}
		else if (cpt_ok == 2 && cpt2_FPGA == 0){
			Dest_msg_SPI[4] = 1;
			cpt_ok = 0; //RAZ
			cpt2_FPGA = 2;
		}

			
}*/





