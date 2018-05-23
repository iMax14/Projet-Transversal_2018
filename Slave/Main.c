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
#include "Config_UART1.h"


#define SYSCLK 22118400L

int Dest_msg_SPI[5] = 0;
signed int Angle_voulu = 0x0000;
char Angle_atteint;
int w,j;

//Variables globales
extern unsigned char msg_CM4[256];
extern unsigned char msg_PointLum[256];
extern unsigned char msg_ServVert[256];
extern unsigned char msg_Vue[256];
extern unsigned char msg_FPGA[256];
int a;


void main (void) {

	EA=0;
	Init_Device();  //Initialisation du microcontrôleur
	Config_Timer2();
	Config_SPI_SLAVE();
	Config_Clock_UART1();
	Config_Clock_UART0();
	Config_UART0();
	Config_UART1();
	EA=1;

/*Fonctions de tests de la connexion SPI
	trame_recue_test(0xD3);
	trame_recue_test(0xD3);
	trame_recue_test(0xA9);
	trame_recue_test(0xBB);
	trame_recue_test(0xFF);
	trame_recue_test(0xFF);*/



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
				/********************
				Appeler la fonction pour piloter le pointeur lumineux 
				en faisant attention que les données reçues sont alternées (que les impair)
				*********************/
			
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
