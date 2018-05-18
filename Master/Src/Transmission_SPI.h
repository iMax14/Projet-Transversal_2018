//------------------------------------------------------------------------------------
// Déclarations de la fonction de la liaison SPI du 8051
//------------------------------------------------------------------------------------


void trame_emise(unsigned char);
unsigned char trame_recue(void);
void debut_trame(unsigned char);
void fin_trame(void);
unsigned char echange_octet(unsigned char);
void echange_trame(unsigned char*, unsigned char,unsigned char);