/*******************************************************************************
Company     : GIBZ
Author      : Ioannis Christodoulakis (IC)
						
Project     : Temperature Controller
Version     : 1.0
Filename	: Temperatur_Regler.c
Created		: 28.08.2018

********************************************************************************
Target Hardware		: ELOB_V1722
Chip type			: ATmega2560
CPU-Clock			: 16.0000 MHz (ext.)
Program Memory		: 256 kByte
Internal RAM		: 8 kByte
EEPROM				: 4 kByte

********************************************************************************
Release Notes:
Version Date		Author	Comment/Reason for change
     

*******************************************************************************/

/******************************************************************************
 INCLUDES
*******************************************************************************/
#include "stdtype.h"
#include "Temperatur_Regler.h"

/******************************************************************************
 DEFINES
*******************************************************************************/
//#define FAKTOR_1000		1000
//
//#define PWM_MIN			0
//#define PWM_MAX			255	
//#define KP				80 //106 Proportional-Faktor
//#define KI				2 // Integral-Faktor
//#define INTEGRAL_MAX	100 //Anti Windup Begrenzung f�r den Regler	


/******************************************************************************
 TYPEDEFS
*******************************************************************************/

/******************************************************************************
 MACROS
*******************************************************************************/

/******************************************************************************
 GLOBALS
*******************************************************************************/
 static float Summe_Regelabweichung = 0;


/******************************************************************************
 FUNCTIONS
*******************************************************************************/

UINT8 Temperatur_Regler_PI (UINT32 Regler_Sollwert, INT32 Regler_Temperatur_aktuell){
 
 float Regler_Sollwert_int = 0;
 float Regler_Temperatur_aktuell_int = 0;
 float Regelabweichung_e = 0;
 float Stellgroesse = 0;
 UINT8 Regler_PWM = 0;
 float test;
		
		//Typkonvertierung in lokale Kopie --> nicht nachvollziebares Verhalten des Compilers
		Regler_Sollwert_int = Regler_Sollwert;
		Regler_Temperatur_aktuell_int = Regler_Temperatur_aktuell;
		
		//Regelabweichung berechnen (Sollwert - Istwert)
		Regelabweichung_e = (Regler_Sollwert_int - Regler_Temperatur_aktuell_int) / FAKTOR_1000;
		
		//Regelabweichung Ausummieren f�r I-Anteil (Integration)
		Summe_Regelabweichung += Regelabweichung_e;		//Integration I-Anteil
		
		//Anti Windup --> begrenzung des I Anteils
		if (Summe_Regelabweichung > INTEGRAL_MAX )
		Summe_Regelabweichung = INTEGRAL_MAX;
		if (Summe_Regelabweichung < 0)
		Summe_Regelabweichung = 0;
		
		//Berechnen der neuen Stellgr�sse nach PI-Regler
		Stellgroesse = KP*Regelabweichung_e + KI*Summe_Regelabweichung;	//Reglergleichung f�r PI
		
		//Berenzung PWM (8-Bit)
		if (Stellgroesse < PWM_MIN)
		Stellgroesse = PWM_MIN;
		if (Stellgroesse > PWM_MAX)
		Stellgroesse = PWM_MAX;

		Regler_PWM = Stellgroesse;

		return Regler_PWM;

}//Temperatur_Regler_PI




