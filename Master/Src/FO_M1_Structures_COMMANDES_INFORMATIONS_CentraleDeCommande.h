#ifndef struct_com_centrale_serializer
#define struct_com_centrale_serializer
typedef unsigned char BYTE;
typedef char SIGNED_BYTE;

///*****************************************************************************************************************************************
// Structure de données pour les commandes transmises par la centrale ce commande
// Les commandes décodées par FO-M1 seront stockées dans une structure de ce type pour être utilisées par FO-M5 
//*****************************************************************************************************************************************

enum Epreuve {Epreuve_non, epreuve1, epreuve2, epreuve3, epreuve4, epreuve5, epreuve6, epreuve7, epreuve8, Fin_Epreuve, Stop_Urgence};
enum Mouvement {Mouvement_non, Avancer, Reculer, Stopper, Rot_90D, Rot_90G, Rot_180D, Rot_180G, Rot_AngD, Rot_AngG, Depl_Coord}; 
enum ACQ_Son {ACQ_non, ACQ_oui};
enum DCT_Obst {DCT_non, oui_180, oui_360};
enum Lumiere {Lumiere_non, Allumer, Eteindre};
enum Servo {Servo_non, Servo_H, Servo_V};
enum Energie{Energie_non,Mesure_I,Mesure_E};
enum Position{Position_non, Init_Position, Demande_Position};
enum Photo{Photo_non,Photo_1,Photo_Multiple,Photo_continue,Photo_stop};
enum Type_Analyse_Obst{Non_obst,Obst_unique, Obst_balayage, Obst_proche_balayage};
enum Detecteur{avant, avant_arriere};
//a modifer
enum Son {non_emission, emission};

typedef struct COMMANDES       // Cette structure contient toutes les commandes envoyées par la centrale de commande
                               // Les variables de type enum correspondent aux commandes
                               // Les variables de type BYTE, UNSIGNED_BYTE ou int correspondent aux paramètres
                
{           
   enum  Epreuve Etat_Epreuve;     //Contient le numéro d'épreuve en cours - Commandes "D" "E" "Q"
   
   enum  Mouvement Etat_Mouvement; //Contient le type de mouvement demandé - Commandes "A" "B" "S" "RD" "RG" "RC" "RA" "G"
                                   // Paramètres des commandes précédentes:     
   BYTE  Vitesse;                  // Paramètre vitesse - Peut aussi être modifié par la commande "TV"
   signed int  Coord_X;	                 // Paramètre coordonnée X
   signed int  Coord_Y;                  // Paramètre coordonnée Y
   signed int   Angle;                    // Paramètre angle
   
   enum  ACQ_Son Etat_ACQ_Son;     // Contient la commande d'acquisition de signaux sonores "ASS"
                                   // Paramètre de la commande précédente:     
   BYTE  ACQ_Duree;                // Paramètre durée de la séquence d'acquisition
	
	//generation de signaux sonores
	//modifié
	enum Son son	;										//son ou pas
	 BYTE	frequence;									// définit par le code fréquence allant de 1 a 99 (mais defini de 1 a 21)
   BYTE duree_son;								// durée du signal sonore
	 BYTE duree_silence;						// duree du silence
	 BYTE nombre_Bips;						// nombre de bips
	
	
  enum	Type_Analyse_Obst	A_Obst; // mou mos ou mob
	enum  DCT_Obst Etat_DCT_Obst;   // Contient la commande de détection d'obstacle "MOB D" "MOB"
  enum	Detecteur Det;							// avant ou avant_arriere                                 
   signed int  DCT_Obst_Resolution;      // Paramètre résolution angulaire de la détection d'obstacle
   //BYTE
   enum  Lumiere Etat_Lumiere;     // Contient la commande d'allumage du pointeur lumineux "L" "LS"
                                   // Paramètres de la commande précédente:     
   BYTE  Lumiere_Intensite;        // Paramètre intensité lumineuse 							  
   BYTE  Lumiere_Duree;            // Paramètre durée d'allumage
   BYTE  Lumiere_Extinction;        // Paramètre durée d'extinction
   BYTE  Lumiere_Nbre;             // Paramètre nombre de cycles d'allumage/extinction
   
   enum  Servo Etat_Servo;        // Contient la commande de positionnement d'un servo "CS C" "CS H" "CS V" 
                                  // Paramètre de la commande précédente:     
   signed int  Servo_Angle;      // Paramètre angle de rotation
   enum  Energie Etat_Energie;    // Contient la commande d'envoi du courant instantané ou de l'energie consommée - Commandes "MI" "ME"
   enum  Position Etat_Position;  // Contient les commandes de gestion de la position, initialisation ou demande d'envoi
		                              // de la position - Commandes: "IPO" "POS"		
   BYTE  Pos_Coord_X;	            // Paramètre coordonnée X pour initialisation de la position
   BYTE  Pos_Coord_Y;             // Paramètre coordonnée Y pour initialisation de la position
   int   Pos_Angle;               // Paramètre angle pour initialisation de la position	 
	 enum  Photo Etat_Photo;        // Contient les commandes de la gestion de la prise de photo - Commandes "PPH" "SPH"
	 BYTE  Photo_Duree;	            // Paramètre durée entre 2 photos
   BYTE  Photo_Nbre;              // Paramètre nombre de photos	 
		 
} OUT_M1;



//*****************************************************************************************************************************************
// Structure de données pour les informations à emettre vers la centrale ce commande
// Les données "Informations" produites par FO-M5  seront stockées dans une structure de ce type pour être utilisées par FO-M1 qui devra
// les encoder en messages ASCII 
//*****************************************************************************************************************************************

enum Invite {Invite_non, Invite_oui};
enum BUT_Mouvement {BUT_Atteint_non, BUT_Atteint_oui};
enum BUT_Servo {BUT_Servo_non, BUT_Servo_H, BUT_Servo_V, BUT_Servo_C};
enum RESULT_Courant{RESULT_Courant_non,RESULT_Courant_oui};
enum RESULT_Energie{RESULT_Energie_non,RESULT_Energie_oui};
enum RESULT_Position{RESULT_Position_non,RESULT_Position_oui};
enum RESULT_DCT_Obst {DCT_Obst_non, DCT_Obst_180_oui, DCT_Obst_360_oui};
enum Aux {Aux_non, Aux_oui};
enum Routage {Initialisation, Servo_Hor, Servo_Vert,Deplacement,Obstacle,Energie,Courant,Gene_Son};


typedef struct INFORMATIONS       // Cette structure contient toutes les informations susceptibles d'être transmises à la centrale de commande
	                                // Les variables de type enum correspondent aux types des informations
                                  // Les variables de type BYTE, UNSIGNED_BYTE ou int correspondent aux paramètres
                
{           
   enum  Invite Etat_Invite;      // Contient l'identificateur information d'invite "I"
                                  // Paramètre de l'information précédente:     
   char  *MSG_Invit;	            //Pointeur sur une chaine de caractère d'invite     
   
   enum  BUT_Mouvement Etat_BUT_Mouvement; // Contient l'identificateur information Arrivée au point spécifié "B"
	                                         // Pas de paramètre associé pour l'information précédente:     
   enum  BUT_Servo Etat_BUT_Servo;         // Contient l'identificateur information Servomoteur positionné "AS C" "AS H" "AS H"
	                                         // Pas de paramètre associé pour l'information précédente:     
	 
   enum  RESULT_DCT_Obst Etat_DCT_Obst;    // Contient l'identificateur information Détection des obstacles "KOB"
                                           // Paramètre de l'information précédente:    
	 char   *Tab_Val_Obst;                    //Pointeur sur un tableau de valeurs de distance
	 BYTE	 Nbre_Val_obst;                    //Nombre de points dans le tableau
	 

	
	 enum RESULT_Courant Etat_RESULT_Courant; // Contient un idendicateur d'information "courant mesuré"
	 unsigned int Mesure_Courant;            // Valeur du courant mesuré en mA   
	 
	 enum RESULT_Energie Etat_RESULT_Energie; // Contient un idendicateur d'information "Energie Consommée"
	 unsigned int Mesure_Energie;            // Valeur de l'énergie consommée depuis le début de l'épreuve
	 
	 enum RESULT_Position Etat_RESULT_Position; // Contient un idendicateur d'information "Position"
   BYTE  Pos_Coord_X;	                     // Information Paramètre coordonnée X
   BYTE  Pos_Coord_Y;                      // Information Paramètre coordonnée Y
   int   Pos_Angle;                        // Information Paramètre angle
	 
   enum   Aux Etat_Aux;                    // Contient l'identificateur information auxiliaires "IA"
                                           // Paramètre de l'information précédente:     
   char  *MSG_Aux;	                       //Pointeur sur une chaine de caractère d'informations auxiliaires  
} IN_M1;
// *******************************************************************************************


// commande serializer



//*****************************************************************************************************************************************
// Structure de données pour les commandes à envoyer à la carte Serializer
// Les commandes produites par FO-M5 (à priori) seront stockées dans une structure de ce type pour être utilisées par FO-M2
// Cette structure ne couvre pas toutes les commandes compréhensibles par la carte Serializer. 
// Seules les commande potentiellement utiles au projet SEM815 sont traitées 
// Commandes Traitées:  Reset - getenc - clrenc - mogo - vpid - digo - dpid - rpid - pids - stop - vel - restore
//*****************************************************************************************************************************************

enum Commande {Commande_non, Reset, Getenc_1, Getenc_2, Getenc_1_2, Clrenc_1, Clrenc_2, Clrenc_1_2, mogo_1, mogo_2, mogo_1_2, Vpid_set, Vpid_read,
               digo_1, digo_2, digo_1_2, Dpid_set, Dpid_read, Rpid_Stinger, Pids, Stop, Vel, Restore };


typedef struct COMMANDES_SERIALIZER  // Cette structure contient toutes les commandes à envoyer au serializer
	                                   // Les variables de type enum correspondent aux commandes
                                    // Les variables de type BYTE, UNSIGNED_BYTE ou int corrspondent aux paramètres
                
{           
   enum  Commande Etat_Commande;   // Contient la commande à transmettre "Reset" "getenc 1" "getenc 2" "getenc 1 2" "clrenc 1" "clrenc 2"
                                   // "clrenc 1 2" "mogo 1" "mogo 1" "mogo 2" "mogo 1 2" "vpid" "digo 1" "digo 2" "digo 1 2" "dpid"
									                 // "rpid s"  "pids" "stop" "vel" "restore"
               
   int   Set_P;                    // paramètre commande vpid dpid
   int   Set_I;                    // paramètre commande vpid dpid
   int   Set_D;                    // paramètre commande vpid dpid
   int   Set_L_A;                  // paramètre commande vpid (L)  dpid (A)
   int   Ticks_mot1;               // paramètre Distance - Encoder Ticks moteur 1 (digo)
   SIGNED_BYTE  Vitesse_Mot1;      // paramètre de vitesse de rotation du moteur 1  (mogo digo)
   int   Ticks_mot2;               // paramètre Distance - Encoder Ticks moteur 2 (digo)
   SIGNED_BYTE  Vitesse_Mot2;      // paramètre de vitesse de rotation du moteur 2 (mogo digo)
} IN_M2;


//*****************************************************************************************************************************************
// Structure de données pour les informations transmises par la carte Serializer
// Les informations Serializer décodées par FO-M2 seront stockées dans une structure de ce type pour être utilisées par FO-M5
// Cette structure ne couvre pas toutes les informations transmissibles par la carte Serializer. 
// Seules les informations potentiellement utiles au projet SEM815 sont traitées 
//*****************************************************************************************************************************************

enum Reponse {Reponse_non, Rep_getenc, Rep_vpid, Rep_dpid, Rep_pids, Rep_vel};


typedef struct INFORMATIONS_SERIALIZER // Cette structure contient toutes les informations envoyées par la carte Serialize
	                                     // Les variables de type enum correspondent aux commandes
                                       // Les variables de type BYTE, UNSIGNED_BYTE ou int corrspondent aux paramètres
                
{  
   enum  Reponse Etat_Response;   	   // Contient le type de réponse reçue à une des commandes suivantes:  "vpid" "dpid" "pids" "vel" 
   int   Read_Val_enc_1;               // information commande getenc1
   int   Read_Val_enc_2;               // information commande getenc2
   
   int   Read_P;                       //   information commande vpid dpid
   int   Read_I;                       //   information commande vpid dpid
   int   Read_D;                       //   information commande vpid dpid
   int   Read_L_A;                     //   information commande vpid (L)  dpid (A)
   BYTE  Read_Pids;                    //   information commande pids
   int   Read_Vitesse_mot1;            //   information commande "velocity" moteur 1
   int   Read_Vitesse_mot2;            //   information commande "velocity" moteur 2
   
} OUT_M2;

#endif
