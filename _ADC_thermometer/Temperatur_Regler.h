/*******************************************************************************
Company     : GIBZ
Author      : Ioannis Christodoulakis (IC)
						
Project     : Temperature Controller
Version     : 1.0
Filename	: Temperatur_Regler.h
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

#ifndef TEMPERATUR_REGLER_H_
#define TEMPERATUR_REGLER_H_

/******************************************************************************
 INCLUDES
*******************************************************************************/
#include "stdtype.h"


/******************************************************************************
 DEFINES
*******************************************************************************/
#define FAKTOR_1000		1000

#define PWM_MIN			0
#define PWM_MAX			255	
#define KP				80 //106 Proportional-Faktor
#define KI				2 // Integral-Faktor
#define INTEGRAL_MAX	100 //Anti Windup Begrenzung f�r den Regler	

/******************************************************************************
 TYPEDEFS
*******************************************************************************/

/******************************************************************************
 MACROS
*******************************************************************************/

/******************************************************************************
 FUNCTION PROTOTYPES
*******************************************************************************/

UINT8 Temperatur_Regler_PI (UINT32 Regler_Sollwert, INT32 Regler_Temperatur_aktuell);






#endif /* TEMPERATUR_REGLER_H_ */


//EOF