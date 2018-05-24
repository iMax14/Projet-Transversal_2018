#include <c8051f020.h>
#include <intrins.h>
#include <string.h>
#include <stdio.h>

#include "Declarations_GPIO.h"
#include "Config_Globale.h"
#include "Timers.h"
#include "Servomoteur_Vertical.h"
#include "Config_SPI.h"
#include "Transmission_SPI.h"
#include "Transmission_UART0.h"
#include "Config_UART0.h"
#include "Pointeur_Lumineux.h"
#include "Config_PCA.h"


#define SYSCLK 22118400L

// Global variables

int Dest_msg_SPI[5] = 0;
signed int Angle_voulu = 0x0000;
char Angle_atteint;
int w,j;

extern unsigned char msg_CM4[256];
extern unsigned char msg_PointLum[256];
extern unsigned char msg_ServVert[256];
extern unsigned char msg_Vue[256];
extern unsigned char msg_FPGA[256];
int a;

int compteur,intensite,nombreCycle;
float dureeAllumage,dureeExtinction;
int cpt=0;

void main (void) {

	EA=0;
	Init_Device();  //Initialisation du microcontrôleur
	Config_Timer2();
	Config_Timer3();
	InitTimer0(); 
	Config_SPI_SLAVE();
	Config_Clock_UART0();
	Config_UART0();
	Config_PCA();
	EA=1;

/*Fonctions de tests de la connexion SPI
	trame_recue_test(0xD3);
	trame_recue_test(0xD3);
	trame_recue_test(0xA9);
	trame_recue_test(0xBB);
	trame_recue_test(0xFF);
	trame_recue_test(0xFF);*/

/*Fonction de tests pour le pilotage du pointeur lumineux
	intensite = 70;
	dureeAllumage = 88;
	dureeExtinction = 22;
	nombreCycle = 1;

	cycleAllumageExtinction(intensite,dureeAllumage,dureeExtinction,nombreCycle);*/


	while (1){
		for(w=0; w<sizeof(Dest_msg_SPI);w++){
			if(Dest_msg_SPI[w] == 1)
				break;
		}
		switch(w){
			case 0 :
				UART_CortexM4(msg_CM4); //On appelle sa fonction associée en lui envoyant son message
				trame_emise(0xEE);
			
				memset(msg_CM4,0,strlen(msg_CM4)); // RAZ du message
				Dest_msg_SPI[w] = 0;
				break;
			case 1 :
				intensite = (int) msg_PointLum[0];
				dureeAllumage = (float) msg_PointLum[2];
				dureeExtinction = (float) msg_PointLum[4];
				nombreCycle = (int) msg_PointLum[6];
			
				cycleAllumageExtinction(intensite,dureeAllumage,dureeExtinction,nombreCycle);
			
				memset(msg_PointLum,0,strlen(msg_PointLum)); // RAZ du message
				Dest_msg_SPI[w] = 0;
				break;
			case 2 :
				if (msg_ServVert[2] == 0xAA){ //L'angle est négatif
					msg_ServVert[0] =~ msg_ServVert[0];//On refait le complément pour bien réceptionner le message qui a été complémenter dans le Master
					Angle_voulu |= msg_ServVert[0]; 
					Angle_voulu += 0xFF00;
					Angle_atteint = CDE_Servo_V(Angle_voulu); //On appelle sa fonction associée en lui envoyant son message
				}
				else{ //L'angle est positif
					Angle_voulu = msg_ServVert[0];
					Angle_atteint = CDE_Servo_V(Angle_voulu); //On appelle sa fonction associée en lui envoyant son message
				}
				trame_emise(Angle_atteint);
				
				memset(msg_ServVert,0,strlen(msg_ServVert)); // RAZ du message
				Dest_msg_SPI[w] = 0;
				break;
			case 3 :
				
				memset(msg_Vue,0,strlen(msg_Vue)); // RAZ du message
				Dest_msg_SPI[w] = 0;
				break;
			case 4 :
				
				memset(msg_FPGA,0,strlen(msg_FPGA)); // RAZ du message
				Dest_msg_SPI[w] = 0;
				break;
			default :
				break;
		}

	}

}


void ISR_Timer2 (void) interrupt 5 {
	TF2 = 0; //Remise à '0' du flag d'overflow
	PWM_servo=!PWM_servo; //On envoie le signal PWM au servomoteur
}


void ISR_SPI (void) interrupt 6 {

		SPIF = 0;	//RAZ du flag d'écriture

		trame_recue(); // On réceptionne le message du Master
		trame_emise(0xEE); // On renvoie un message au Master pour lui accuser bonne réception

}

/******Cette fonction d'interruption permet d'incrémenter un compteur toutes les 10ms
			 afin de pouvoir gérer le temps de ON et de OFF du pointeur ******/
void ISR_Timer3 (void) interrupt 14 {

	TMR3CN &= 0x04; //Remise � '0' du flag d'overflow
	cpt++;
}

