//------------------------------------------------------------------------------------
// Déclarations des fonctions de l'UART1 du 8051
//------------------------------------------------------------------------------------

void CFG_UART1(void);
void init_Serial_Buffer1(void);
unsigned char serOutchar1(char c);
char serInchar1(void); 
unsigned int serInchar_Bin1(void);
unsigned char serOutstring1(char *buf);
void CFG_Clock_UART1(void);
