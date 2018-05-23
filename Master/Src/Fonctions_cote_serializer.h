//------------------------------------------------------------------------------------
// Déclarations des fonctions du Serializer du 8051
//------------------------------------------------------------------------------------



#ifndef serializer
#define serializer

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "FO_M1_Structures_COMMANDES_INFORMATIONS_CentraleDeCommande.h"

int conv_distance_ticks(int d);
int conv_angle_ticks(int angle);
void itos(int i, char * ret);
void formate_serializer(struct COMMANDES_SERIALIZER com, char * ret);
struct COMMANDES_SERIALIZER transcode_commande_to_serializer (struct COMMANDES com) ;
void true_ticks(int ticks, char * ret);

#endif