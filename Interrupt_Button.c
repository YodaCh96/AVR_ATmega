/*******************************************************************************
Firma        : Siemens Schweiz AG - Berufsbildung Elektronik
Autor        : Ioannis Christodoulakis

Projekt      : Vorlage_Uart_EinAuslesen
Version      : 2.0
Dateiname    : Vorlage_Uart_EinAuslesen.c
Erstelldatum : 13.02.2018

********************************************************************************
Chip type         : ATmega2560
CPU-Clock         : 1.0000 MHz
Programm Memory   : 16 kByte
Internal RAM      : 1 kByte
EEPROM            : 512 Bytes
Data Stack size   : 16 Bytes

********************************************************************************
Datum             Vers.    Kommentar / Änderungsgrund
13.02.2018        1.0      Erstellung/ fertigstellung ohne Funktion
15.02.2018		  2.0	   Fertigstellung mit Funktion

*******************************************************************************/

/*------------------ Definitionen für die Delayfunktion-----------------------*/ 
#define F_CPU 16000000UL
/* CPU-Clock muss vor der Einbindung von delay.h definiert werden, da dieser
   sonst bereits definiert wurde und darum nicht übernommen würde die
   Wartefunktionen werden mittels _delay_ms(), oder _delay_us() aufgerufen    */

/*------------------------------- Einbindungen -------------------------------*/
#include <avr/io.h>                 // Enthält I/O-Definitionen
#include <util/delay.h>              // Enthält _delay_ms() und _delay_us()
#include <stdint.h>
#include <avr/interrupt.h>			// Enthält Interrupt-Funktionen
#include "Pruefziffer_F.h"			//Pruefziffer Funktion

/*--------------------- Konstanten & Definitionen ----------------------------*/

// Makros
#define		SETBIT(Adr,Bit)				(Adr |= (1<<Bit))
#define		CLEARBIT(Adr,Bit)			(Adr &= ~(1<<Bit))
#define		CHECKBIT(Adr,Bit)			(Adr & (1<<Bit))
#define		INVERTBIT(Adr,Bit)			(Adr ^= (1<<Bit))




// I/O's definieren
#define		TASTER			PINJ
#define		SCHALTER		PINC
#define		LEDS			PORTA



/*---------------------------- Globale Variablen -----------------------------*/
uint8_t Zaehler = 0;



/*--------------------------------Funktionen--------------------------------*/


/*----------------------------- Hauptprogramm --------------------------------*/
int main(void){
	  /* I/O-Konfigurationen*/
	  DDRA = 0xFF;					// PORTA als Ausgang definieren(LEDS)
	 
	  DDRJ = 0x00;                  // PORTJ als Eingang definieren(Taster)
	 
	/* lokale Variablendefinition*/

	

	
	
	PCMSK1 = 0b00000010;
	PCICR  = 0b00000010;
	
	sei(); // set interrupt active
	
/* Eingabe ------------------- */
  while(1)
  {
  LEDS = Zaehler;
  _delay_ms(50);
  
  }//while1
}//main

// Interrupt Service Routine ISR zum UART Receive Complete Interrupt



ISR(PCINT1_vect){
	
Zaehler++;
	
}