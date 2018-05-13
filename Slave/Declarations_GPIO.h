//------------------------------------------------------------------------------------
// Déclarations des GPIOs Projet Transversal 2018
//------------------------------------------------------------------------------------
//
// AUTH: ME
// DATE: 05/05/18
// Target: C8051F02x
//
// Tool chain: KEIL Eval 'c'
//


//-----------------------------------------------------------------------------
// 16-bit SFR Definitions for 'F02x
//-----------------------------------------------------------------------------

sfr16 DP       = 0x82;                 // data pointer
sfr16 TMR3RL   = 0x92;                 // Timer3 reload value
sfr16 TMR3     = 0x94;                 // Timer3 counter
sfr16 ADC0     = 0xbe;                 // ADC0 data
sfr16 ADC0GT   = 0xc4;                 // ADC0 greater than window
sfr16 ADC0LT   = 0xc6;                 // ADC0 less than window
sfr16 RCAP2    = 0xca;                 // Timer2 capture/reload
sfr16 T2       = 0xcc;                 // Timer2
sfr16 RCAP4    = 0xe4;                 // Timer4 capture/reload
sfr16 T4       = 0xf4;                 // Timer4
sfr16 DAC0     = 0xd2;                 // DAC0 data
sfr16 DAC1     = 0xd5;                 // DAC1 data


// Elements standard de port 
sbit P0__2 = P0^2;
sbit P0__3 = P0^3;
sbit P0__4 = P0^4;
sbit P0__5 = P0^5;
sbit P2__0 = P2^0;

// Eléments pour le SLAVE
sbit SCK_SPI = P0^2;//Broche A12
sbit MISO_SPI = P0^3;//Broche C11
sbit MOSI_SPI = P0^4;//Broche B11
sbit NSS__SPI = P0^5;//Broche A11

sbit PWM_servo = P2^0; //Broche B7





