/*******************************************************************************
Company     : GIBZ
Author      : Ioannis Christodoulakis (IC)
						
Project     : Temperature Controller
Version     : 1.0
Filename	: main.c
Created		: 14.08.2018

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

/*------------------ Definitionen f�r die Delayfunktion-----------------------*/
#define F_CPU 16000000UL
/* CPU-Clock muss vor der Einbindung von delay.h definiert werden, da dieser
sonst bereits definiert wurde und darum nicht �bernommen w�rde die
Wartefunktionen werden mittels _delay_ms(), oder _delay_us() aufgerufen    */

/******************************************************************************
 INCLUDES
*******************************************************************************/
#include <avr/io.h>
#include <avr/delay.h>              // Enth�lt _delay_ms() und _delay_us()
#include <avr/interrupt.h>			// Enth�lt Interrupt-Funktionen
#include "stdtype.h"
#include "elob_hw_init.h" 
#include "Temperatur_Regler.h"

/******************************************************************************
 DEFINES
*******************************************************************************/
#define ASCII_0			0x30	//ASCII wert f�r Zahl 0
#define DIVISOR_10		10	
#define OVERSAMPLING	10 // oversampling rate
#define FAKTOR_MILLI	1000
#define TEMP_MIN        -1000 //Unit: �C/1000
#define TEMP_MAX        9900  //Unit: �C/1000

#define ABTASTZEIT		500 //ms
#define SOLLWERT		40	// Sollwert f�r den Regler in �C
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
 GLOBALS
*******************************************************************************/
UINT8 Vorzeichen = 0;
UINT8 Zehntausender = 0;
UINT8 Tausender = 0;
UINT8 Hunderter = 0;
UINT8 Zehner = 0;
UINT8 Einer = 0;

UINT32 ADC_Wert = 0;

// NTC Temp table from -10�C to 99�C
const INT16 NTC_TEMP_TABLE[110] ={
	176, 184, 192, 200, 208, 216, 224, 233, 242, 250,
	260, 269, 278, 287, 297, 307, 317, 326, 336, 347,
	357, 367, 377, 387, 398, 408, 419, 429, 439, 450,
	460, 470, 481, 491, 501, 511, 521, 531, 541, 551,
	561, 570, 580, 589, 599, 608, 617, 626, 635, 643,
	652, 660, 668, 676, 684, 692, 700, 708, 715, 722,
	729, 736, 743, 750, 756, 763, 769, 775, 781, 787,
	792, 798, 804, 809, 814, 819, 824, 829, 834, 838,
	843, 847, 851, 855, 859, 863, 867, 871, 875, 878,
	882, 885, 888, 891, 895, 898, 901, 903, 906, 909,
	912, 914, 917, 919, 922, 924, 926, 928, 931, 933
};


/******************************************************************************
 FUNCTIONS
*******************************************************************************/
void dez_to_ascii(INT32 Data){

	INT32 Divisor = DIVISOR_10;
	INT8 NegPosKorrektur = 1;
	
	if (Data < 0){
		
		Vorzeichen = '-'; // ASCII 0x2D "-"
		Divisor *= -1;
		NegPosKorrektur = -1;
		
	}//if negativ
	else{
		
		Vorzeichen = '+'; // ASCII 0x2B "+"
		
	}//else positiv
	
	Einer = (Data % Divisor)*NegPosKorrektur + ASCII_0;
	
	Data /=DIVISOR_10;
	
	Zehner = (Data % Divisor)*NegPosKorrektur + ASCII_0;
	
	Data /=DIVISOR_10;
	
	Hunderter = (Data % Divisor)*NegPosKorrektur + ASCII_0;
	
	Data /=DIVISOR_10;

	Tausender = (Data % Divisor)*NegPosKorrektur + ASCII_0;
	
	Data /=DIVISOR_10;

	Zehntausender = (Data % Divisor)*NegPosKorrektur + ASCII_0;
	
}//dez_to_ascii
			

/******************************************************************************
 MAIN ROUTINE WITH ENDLESS LOOP
*******************************************************************************/
int main(void) {

	UINT32  AnalogValue = 0;
	UINT32  ReferenceValue = 0;
	UINT8   Temp_Counter = 0;
	UINT8   Sample_counter = 0;
	UINT32  Temp_Interpolation;
	INT32   Temperatur_Ist;
	UINT8   PWM_Berechnet = 0;
	
	
	float Temperatur_aktuell = 0;
	UINT32 Sollwert = 50000; //m�C
	float Regelabweichung_e = 0;
	float Summe_Regelabweichung = 0;

	float Stellgroesse = 0;

	
	//init of the hardware of the ELOB-Board	
	elob_hw_init();
	
	sei(); // set interrupt active
	
	//set the pwm for the step response
	OCR2A = 0; // 100%
	
	// read the first ADC Value and scrap it
	//Get new ADC-Value
	ADCSRA |= (1 << ADSC); //start conversion
	//while(!ADC_Wert) //wait until adc value was stored in the ISR
	while( ADCSRA & (1 << ADSC) )
	
    // main loop
    while (1) {	
		
		//Reset Temp_Counter		
		Temp_Counter = 0;
		AnalogValue = 0;
		Temp_Interpolation = 0;
		Temperatur_Ist = 0;
		
		//Abtastzeit (Sampling Time) 0.5s
		_delay_ms(ABTASTZEIT);
		
		for(Sample_counter = 0; Sample_counter < OVERSAMPLING; Sample_counter++){
			
			ADC_Wert = 0;
			
			//Get new ADC-Value
			ADCSRA |= (1 << ADSC); //start conversion
		
			//while(!ADC_Wert) //wait until adc value was stored in the ISR
			while( ADCSRA & (1 << ADSC) )
			
			ADC_Wert = ADCW; //read ADC value
		
			//build the sum of the ADC values 
			AnalogValue += ADC_Wert;
			
		}//for oversampling
		
		//divide for the Oversampling
		AnalogValue /= OVERSAMPLING;
		

		//Temperature
		//find the temp. value for this adc-value
		//for (Temp_Counter = 0; NTC_TEMP_TABLE[Temp_Counter] < AnalogValue ; Temp_Counter++){
		//}//for search in lookup table
		while (NTC_TEMP_TABLE[Temp_Counter] < ADC_Wert){
			Temp_Counter++;
		}
		//decrement the counter if not 0
		if (Temp_Counter)
			Temp_Counter --;	
			 
		//linear temp interpolation
		Temp_Interpolation = ( (AnalogValue - NTC_TEMP_TABLE[Temp_Counter]) *100 ) / ( NTC_TEMP_TABLE[Temp_Counter+1] - NTC_TEMP_TABLE[Temp_Counter] );			 
			 
		//calculate an scale the temperature to milli-grade celsius
		Temperatur_Ist = ((Temp_Counter*100 + TEMP_MIN + Temp_Interpolation) * 10);
			
		PWM_Berechnet = Temperatur_Regler_PI (Sollwert, Temperatur_Ist);
	
		//PWM gem�ss Regler setzen
		OCR2A = PWM_Berechnet;
		
		//ASCII Umwandlung
		dez_to_ascii(Temperatur_Ist);
			
		//UART Uebertragung
		while ( !( UCSR1A & (1<<UDRE1)) ); // warten auf leeren  transmit buffer
		UDR1 = Zehntausender;
		while ( !( UCSR1A & (1<<UDRE1)) ); // warten auf leeren  transmit buffer
		UDR1 = Tausender;
		while ( !( UCSR1A & (1<<UDRE1)) ); // warten auf leeren  transmit buffer
		UDR1 = Hunderter;
		while ( !( UCSR1A & (1<<UDRE1)) ); // warten auf leeren  transmit buffer
		UDR1 = Zehner;
		while ( !( UCSR1A & (1<<UDRE1)) ); // warten auf leeren  transmit buffer
		UDR1 = Einer;
		while ( !( UCSR1A & (1<<UDRE1)) ); // warten auf leeren  transmit buffer
		UDR1 = ';';
		
		
		
    }//while (main loop)
	
}//main



/******************************************************************************
 INTERRUP SERVICE ROUTINES
*******************************************************************************/

// Interrupt Service Routine ISR fuer den ADC complete conversion interrupt
ISR(ADC_vect){
		
		ADC_Wert = ADCW; //read ADC value
		
}//ISR TIMER5_COMPA_vect


//EOF
