#include <c8051f020.h>
#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "UART0_RingBuffer_lib.h"
#include <string.h>
#include "Servomoteur_horizontal.h"
#include "Transmission_SPI.h"
#include "Lien_Structure_commande.h"
#include "Generation_son.h"

void son_sonore(struct COMMANDES com){
	char rep[4]=0;
	unsigned char commande_SPI = 0x00;
	unsigned char taille_trame = 4;

	commande_SPI = 0xD1;
	rep[0] = echange_octet(com.frequence);
	rep[1] = echange_octet(com.duree_son);
	rep[2] = echange_octet(com.duree_silence);
	rep[3] = echange_octet(com.nombre_Bips);

	echange_trame(rep,taille_trame,commande_SPI);
	tempo_emiss();
}
