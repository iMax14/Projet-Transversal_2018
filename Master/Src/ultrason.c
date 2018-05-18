#include <c8051f020.h>
#include <intrins.h>
#include <string.h>

#include "ultrason.h"
#include "Declarations_GPIO.h"
#include "Timers.h"

#define SYSCLK 22118400L


float MES_Dist_AV(void){	
	float Delta;
	float TL0_tmp;
	float TH0_tmp ;
	float Duree ;
	float Dist;
	float Duree2;
	int i = 0;
	float RegistreTemp;
	
	initialisation_Timer0();
	Trig_ultrason_av = 1;
	TL0_tmp = 0;
	TH0_tmp = 0;
	
	while(i<= 5){ // envoie implusion
		_nop_();
		i = i + 1;
	}
		
	Trig_ultrason_av = 0;
	TR0 =1;
	
	while(Echo_ultrason_av == 0 && TF0 == 0 ); // si détection rising edge impulstion
		
	if(TF0 == 1){
		Dist = 0;
	}// overflow		
	else
	{
		TL0_tmp = TL0;
		TH0_tmp = TH0;
		while(Echo_ultrason_av == 1){}
		TR0 = 0; // Stop timer2
		RegistreTemp =  256.f * (float)TH0_tmp +  (float)TL0_tmp; 
		Delta =  256.f * (float)TH0 -  (float)TL0;
		
		Duree = ((Delta - RegistreTemp ) ) ; // Duree impulsion ECHO en us
		Duree2 = (float)Duree/22.21184f;
		Dist = Duree2 / 37.5f;
		TF0 = 0; 
	}
		
	return Dist ; 
}

float MES_Dist_AR(void)
{	
	float Delta;
	float TL0_tmp;
	float TH0_tmp ;
	float Duree ;
	float Dist;
	float Duree2;
	int i = 0;
	float RegistreTemp;
	
	initialisation_Timer0();
	Trig_ultrason_ar = 1;
	TL0_tmp = 0;
	TH0_tmp = 0;
	
	while(i<= 5){ // envoie implusion
		_nop_();
		i = i + 1;
	}
		
	Trig_ultrason_ar = 0;
	TR0 =1;
	
	while(Echo_ultrason_ar == 0 && TF0 == 0 ); // si détection rising edge impulstion
		
	if(TF0 == 1){
		Dist = 0;
	}// overflow		
	else
	{
		TL0_tmp = TL0;
		TH0_tmp = TH0;
		while(Echo_ultrason_ar == 1){}
		TR0 = 0; // Stop timer2
		RegistreTemp =  256.f * (float)TH0_tmp +  (float)TL0_tmp; 
		Delta =  256.f * (float)TH0 -  (float)TL0;
		
		Duree = ((Delta - RegistreTemp ) ) ; // Duree impulsion ECHO en us
		Duree2 = (float)Duree/22.21184f;
		Dist = Duree2 / 37.5f;
		TF0 = 0; 
	}
		
	return Dist ; 
}






