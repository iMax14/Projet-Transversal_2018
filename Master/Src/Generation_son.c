#include <c8051f020.h>
#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "UART0_RingBuffer_lib.h"
#include <string.h>
#include "Servomoteur_horizontal.h"
#include "Transmission_SPI.h"
#include "Lien_Structure_commande.h"
#include "Generation_son.h"

void son_sonore(struct COMMANDES com){
	unsigned char rep[4]=0;
	unsigned char commande_SPI = 0x00;
	unsigned char taille_trame = 4;

	commande_SPI = 0xD1;
	rep[0] = com.frequence;
	rep[1] = com.duree_son;
	rep[2] = com.duree_silence;
	rep[3] = com.nombre_Bips;

	echange_trame(rep,taille_trame,commande_SPI);
	tempo_emiss();
}
