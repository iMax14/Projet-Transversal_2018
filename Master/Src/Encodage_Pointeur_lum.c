#include <c8051f020.h>
#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"
#include "UART0_RingBuffer_lib.h"
#include <string.h>
#include "Transmission_SPI.h"
#include "Lien_Structure_commande.h"
#include "Encodage_Pointeur_lum.h"

void pointeur_lum(struct COMMANDES com){
	char rep[4]=0;
	unsigned char commande_SPI = 0xD2;
	unsigned char taille_trame = 4;

	rep[0] = com.Lumiere_Intensite;
	rep[1] = com.Lumiere_Duree;
	rep[2] = com.Lumiere_Extinction;
	rep[3] = com.Lumiere_Nbre;

	echange_trame(rep,taille_trame,commande_SPI);
	tempo_emiss();
}
