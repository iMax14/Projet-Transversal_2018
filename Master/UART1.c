#include <c8051f020.h>
#include <stdio.h>
#include <string.h>
#include <intrins.h>

#include "ConfigUART1.h"
#include "ConfigUART0.h"


//*************************************************************************************************

#ifndef CFG_Globale
   #define CFG_Globale
   #include <CFG_Globale.h>
#endif


//************************************************************************************************
// Paramètresd modifiables
//*************************************************************************************************
#define       MAX_BUFLEN 32 // Taille des buffers de données

//*************************************************************************************************
// DEFINITION DES MACROS DE GESTION DE BUFFER CIRCULAIRE
//*************************************************************************************************

// Structure de gestion de buffer circulaire
	//rb_start: pointeur sur l'adresse de début du buffer 
	// rb_end: pointeur sur l'adresse de fin du buffer	
	// rb_in: pointeur sur la donnée à lire
	// rb_out: pointeur sur la case à écrire
		
#define RB_CREATE(rb, type) \
   struct { \
     type *rb_start; \	   
     type *rb_end; \	   
     type *rb_in; \
	   type *rb_out; \		
	  } rb

//Initialisation de la structure de pointeurs 
// rb: adresse de la structure
// start: adresse du premier élément du buffer 
// number: nombre d'éléments du buffer - 1	(le "-1" n'est à mon avis pas nécessaire)
#define RB_INIT(rb, start, number) \
         ( (rb)->rb_in = (rb)->rb_out= (rb)->rb_start= start, \
           (rb)->rb_end = &(rb)->rb_start[number] )

//Cette macro rend le buffer circulaire. Quand on atteint la fin, on retourne au début
#define RB_SLOT(rb, slot) \
         ( (slot)==(rb)->rb_end? (rb)->rb_start: (slot) )

// Test: Buffer vide? 
#define RB_EMPTY(rb) ( (rb)->rb_in==(rb)->rb_out )

// Test: Buffer plein?
#define RB_FULL(rb)  ( RB_SLOT(rb, (rb)->rb_in+1)==(rb)->rb_out )

// Incrementation du pointeur dur la case à écrire
#define RB_PUSHADVANCE(rb) ( (rb)->rb_in= RB_SLOT((rb), (rb)->rb_in+1) )

// Incrémentation du pointeur sur la case à lire
#define RB_POPADVANCE(rb)  ( (rb)->rb_out= RB_SLOT((rb), (rb)->rb_out+1) )

// Pointeur pour stocker une valeur dans le buffer
#define RB_PUSHSLOT(rb) ( (rb)->rb_in )

// pointeur pour lire une valeur dans le buffer
#define RB_POPSLOT(rb)  ( (rb)->rb_out )


//*************************************************************************************************


/* Transmission and Reception Data Buffers */
static char  xdata outbuf[MAX_BUFLEN];     /* memory for transmission ring buffer #1 (TXD) */
static char  xdata inbuf [MAX_BUFLEN];     /* memory for reception ring buffer #2 (RXD) */
static  bit   TXactive = 0;             /* transmission status flag (off) */

/* define out (transmission)  and in (reception)  ring buffer control structures */
static RB_CREATE(out,unsigned char xdata);            /* static struct { ... } out; */
static RB_CREATE(in, unsigned char xdata);            /* static struct { ... } in; */

//**************************************************************************************************
//**************************************************************************************************
void UART1_ISR(void) interrupt 20{
 
	//static unsigned int cp_tx = 0;
	//static unsigned int cp_rx = 0;
		
	//char TI1;
	//TI1=SCON1;
	//TI1&=0x02;

  if ( SCON1 & 0x02) // On peut envoyer une nouvelle donnée sur la liaison série
  { 
  	if(!RB_EMPTY(&out)) {
			SBUF1 = *RB_POPSLOT(&out);      /* start transmission of next byte */
			RB_POPADVANCE(&out);            /* remove the sent byte from buffer */
			//cp_tx++;
  	}
  	else TXactive = 0;                 /* TX finished, interface inactive */
			SCON1 &= 0xFD;
		
  }
  else // RI1 à 1 - Donc une donnée a été reçue
  {
		if(!RB_FULL(&in)) {                   // si le buffer est plein, la donnée reçue est perdue
     	*RB_PUSHSLOT(&in) = SBUF1;        /* store new data in the buffer */
		  RB_PUSHADVANCE(&in);               /* next write location */
			//cp_rx++;
	 }
   SCON1 &= 0xFE; 
  }
}
// **************************************************************************************************
// init_Serial_Buffer: Initialisation des structuresde gestion des buffers transmission et reception
// *************************************************************************************************
//**************************************************************************************************
void init_Serial_Buffer1(void){
    RB_INIT(&out, outbuf, MAX_BUFLEN-1);           /* set up TX ring buffer */
    RB_INIT(&in, inbuf,MAX_BUFLEN-1);             /* set up RX ring buffer */
}
// **************************************************************************************************
// SerOutchar: envoi d'un caractère dans le buffer de transmission de la liaison série
// *************************************************************************************************
unsigned char serOutchar1(char c){

  if(!RB_FULL(&out))  // si le buffer n'est pas plein, on place l'octet dans le buffer
  {                 
  	*RB_PUSHSLOT(&out) = c;               /* store data in the buffer */
  	RB_PUSHADVANCE(&out);                 /* adjust write position */

  	if(!TXactive) {
			TXactive = 1;                      /* indicate ongoing transmission */
			SCON1 |= 0x02;
			//TI0 = 1;//   Placer le bit TI à 1 pour provoquer le déclenchement de l'interruption
  	}
		return 0;  // opération correctement réalisée 
  }
  else 
		return 1; // opération échouée - Typiquement Buffer plein
}
// ************************************************************************************************
//  serInchar: 	lecture d'un caractère dans le buffer de réception de la liaison série
//  Fonction adaptée pour la réception de codes ASCII - La réception de la valeur binaire 0
//  n'est pas possible (conflit avec le code 0 retourné si il n'y a pas de caractère reçu)
// ************************************************************************************************
char serInchar1(void){
	char c;

  if (!RB_EMPTY(&in))
  {                 /* wait for data */
  	c = *RB_POPSLOT(&in);                 /* get character off the buffer */
 	  RB_POPADVANCE(&in);                   /* adjust read position */
  	return c;
  }
  else 
		return 0;
}
// ************************************************************************************************
//  serInchar_Bin: 	lecture d'un caractère dans le buffer de réception de la liaison série
//  Fonction adaptée pour la réception de codes Binaires - Cette fonction retourne un entier. 
//  L'octet de poids faible correspond au caractère reçu, l'octet de poids fort, s'il est nul indique 
//  qu'aucun caractère n'a été reçu.
//  
// ************************************************************************************************
unsigned int serInchar_Bin1(void) {
	char c;
	unsigned int return_code = 0;
	 
  if (!RB_EMPTY(&in)){                
    // un caractère au moins est dans le buffer de réception
  	c = *RB_POPSLOT(&in);                 /* get character off the buffer */
 	  RB_POPADVANCE(&in);                   /* adjust read position */
  	return 0xFF00+c;
  }
	// pas de caractère dans le buffer de réception.
  else 
		return return_code;
}
// *************************************************************************************************
// serOutstring:  Envoi d'une chaine de caractère dans le buffer de transmission
// ************************************************************************************************
unsigned char serOutstring1(char *buf) {
	unsigned char len,code_err=0;

  for(len = 0; len < strlen(buf); len++)
     code_err +=serOutchar1(buf[len]);
  return code_err;
}


/*void CFG_Clock_UART1(void){
	TCON &=0x7F;
	CKCON |= 0x10;      // T1M: Timer 1 use the system clock.
	PCON |= 0x10;
	PCON &= 0xF7;


	TH1=0xB8;
	TH1=0xF4;
	TL1=0x00;
	
	TMOD |= 0x20;       //  Timer1 CLK = system clock
	TMOD &= 0x2F;			  // Timer1 configuré en timer 8 bit avec auto-reload	
	ET1 = 0;				   // Interruption Timer 1 dévalidée
	TR1 = 1;				   // Timer1 démarré
}

void CFG_UART1(void){
	T4CON &= 0xCF ;
	PCON |= 0x90; // SMOD0=1 On ne divise pas le Baud rate de l'uart0 par 2
	PCON &=0xB7; //SSTAT0=0  Donne l'accès à SM20 - SM000
	SCON1 = 0x70;   // Mode 1 - Check Stop bit - Reception validée
	TI1 = 1;        // Transmission: octet transmis (prêt à recevoir un char
								// pour transmettre			
	IE = 1;        // interruption UART0 autorisée	
	EIE2 |= 0x40;
	SCON1 |= 0x72;
	SCON1 &= 0x7E;

}*/
	
void CFG_UART1(void){
	T4CON &= 0xCF ;
	PCON |= 0x90; // SMOD0=1 On ne divise pas le Baud rate de l'uart0 par 2
	PCON &=0xB7; //SSTAT0=0  Donne l'accès à SM20 - SM000
	SCON1 |= 0x72;   // Mode 1 - Check Stop bit - Reception validée
				// Transmission: octet transmis (prêt à recevoir un char pour transmettre			
	EIE2 |= 0x40;        // interruption UART0 autorisée	
}
