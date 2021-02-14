/*******************************************************************************
Firma        : Siemens Schweiz AG - Berufsbildung Elektronik
Autor        : Ioannis Christodoulakis

Projekt      : Akku Rasenmäher
Version      : 1.0
Dateiname    : Akku Rasenmäher.c
Erstelldatum : 10.06.2018

********************************************************************************
Chip type         : ATmega2560
CPU-Clock         : 16.0000 MHz (externer Quartz)
Programm Memory   : 256 kByte
Internal RAM      : 8 kByte
EEPROM            : 4 kByte
Data Stack size   : 16 Byte

********************************************************************************
Datum             Vers.    Kommentar / Änderungsgrund
10.06.2018        1.0      Erstellung

*******************************************************************************/

/*------------------ Definitionen für die Delayfunktion-----------------------*/
#define F_CPU 16000000UL
/* CPU-Clock muss vor der Einbindung von delay.h definiert werden, da dieser
   sonst bereits definiert wurde und darum nicht übernommen würde             */

/*------------------------------- Einbindungen -------------------------------*/
#include <avr/io.h>                 // Enthält I/O-Definitionen
#include <avr/delay.h>              // Enthält _delay_ms() und _delay_us()

/*---------------------------- Globale Variablen -----------------------------*/
unsigned char ImpulsDauer = 2;
unsigned char PeriodenDauer = 4;
unsigned char BlinkZaehler = 0;

unsigned char Signal_Schalter = 0;
unsigned char Signal_Taster = 0;
unsigned char LED_Anzeige = 0;
unsigned char Start_Taster = 0;
unsigned char Betrieb_Taster = 0;
unsigned char Rotor_Status = 0;
unsigned char Akku_Ladestand = 0;


char TastenAuswertung;
/* Diese Variable wird in der Funktion "PositiveFlankenerkennung" verwendet,
   welche bei Erkennung einer positiven Flanke am Tasten-Port das entsprechende
   Bit der Variable TastenAuswertung auf HIGH setzt */

/*--------------------- Konstanten & Definitionen ----------------------------*/

// Konstanten

// Makros
#define SETBIT(Adr,Bit)         (Adr |= (1<<Bit))
#define CLEARBIT(Adr,Bit)       (Adr &= ~(1<<Bit))
#define CHECKBIT(Adr,Bit)       (Adr & (1<<Bit))
#define INVERTBIT(Adr,Bit)      (Adr ^= (1<<Bit))


// I/O's definieren (LEDs, Taster und Schalter sind auf dem Elo-Board fix)
#define LEDS      PORTA
#define TASTER    PINJ
#define SCHALTER  PINC

#define MOTOR 0x80;
#define AKKU 0x0F;
#define ROTOR 0x80;
#define START 0x10;
#define BETRIEB 0x0F;

/*--------------------------- Funktionsprototypen ----------------------------*/
void PositiveFlankenerkennung(void);
void WaitMilliseconds(int MilliSeconds);
void blink(void);

/*----------------------------- Hauptprogramm --------------------------------*/

int main(void)
{
  // I/O-Konfigurationen
  DDRA = 0xFF;                    // LED-Port als Ausgang definieren

  while(1){
	  
	Signal_Schalter = SCHALTER;
	Akku_Ladestand = Signal_Schalter & AKKU;
	Rotor_Status = Signal_Schalter & ROTOR;
	
	Signal_Taster = TASTER;
	Start_Taster = Signal_Taster & START;
	Betrieb_Taster = Signal_Taster & BETRIEB;
	
	if(Start_Taster && Betrieb_Taster){
		LED_Anzeige |= MOTOR;
	}
	
	if(!Betrieb_Taster || Rotor_Status || !Akku_Ladestand){
		LED_Anzeige &= ~MOTOR;
	}
	
	if(Akku_Ladestand){
		LED_Anzeige &= ~AKKU;
		LED_Anzeige |= Akku_Ladestand;
	}else{
		LED_Anzeige ^= 0x01;
	}
	
	
	LEDS = LED_Anzeige;
	  
   WaitMilliseconds(100);
  } // endlos                         
} // main 

/*------------------------------- Funktionen ---------------------------------*/
void PositiveFlankenerkennung(void){
  static char TastenVorher = 0;	   // Beim ersten Aufruf mit Null initialisieren
  char TasteAktuell;

  TasteAktuell = TASTER;              // Tasten einlesen und den Wechsel von
  TastenAuswertung = TasteAktuell & ~TastenVorher;   // LOW auf HIGH erkennen
  TastenVorher = TasteAktuell;        // Aktuelles Muster in Erinnerung behalten
}

void WaitMilliseconds(int MilliSeconds){
  for(int i = 0; i < MilliSeconds; i++){
    _delay_ms(1);
  }  
}

