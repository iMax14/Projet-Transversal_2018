//Fonction d'initiation del'uart
	void CFG_Clock_UART0(void);
	void CFG_UART0(void);
	void UART0_ISR(void); 
	void init_Serial_Buffer(void);
	unsigned char serOutchar(char c);
	char serInchar(void);
	unsigned int serInchar_Bin(void);
	unsigned char serOutstring(char *buf);
