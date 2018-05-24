#include <c8051f020.h> 
#include <math.h>
#include <stdio.h>
#include <intrins.h>

#include "Declarations_GPIO.h"
#include "Config_PCA.h"

void Config_PCA(void){
	
	PCA0MD |= 0x02; //On utilise la SYCLK/12
	PCA0CPM0 |= 0xC2; //On active le mode PWM 16 bits 
	PCA0CN |= 0x40; //On active le PCA
}

