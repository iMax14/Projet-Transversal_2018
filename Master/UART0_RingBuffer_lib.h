//------------------------------------------------------------------------------------
// Déclarations des fonctions de l'UART0 du 8051
//------------------------------------------------------------------------------------


void init_Serial_Buffer(void);
unsigned char serOutchar(char c);
char serInchar(void); 
unsigned int serInchar_Bin(void);
unsigned char serOutstring(char *buf);
void CFG_Clock_UART(void);
void cfg_UART0_mode1(void);
	