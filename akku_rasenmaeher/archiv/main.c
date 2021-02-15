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

unsigned char BlinkZaehler = 0;
unsigned char ImpulsDauer = 2;
unsigned char PeriodenDauer = 4;

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

//Inputs
#define START_TASTER (TASTER & 0x10)
#define BETRIEB_TASTER (TASTER & 0x0F)
#define  ROTOR_BLOCK (SCHALTER & 0x80)
#define AKKU_LADESTAND (SCHALTER & 0x0F)

//Outputs
#define AKKU_LEER_ON SETBIT(LEDS,0)
#define AKKU_LEER_OFF CLEARBIT(LEDS,0)
#define MOTOR_ON SETBIT(LEDS,7)
#define MOTOR_OFF CLEARBIT(LEDS,7)

/*--------------------------- Funktionsprototypen ----------------------------*/
void PositiveFlankenerkennung(void);
void WaitMilliseconds(int MilliSeconds);
void blink(void);

/*----------------------------- Hauptprogramm --------------------------------*/

int main(void)
{
	//lokale Variablen
	unsigned char Motor_flag = 0;
	
  // I/O-Konfigurationen
  DDRA = 0xFF;                    // LED-Port als Ausgang definieren

  while(1){
  
	if(ROTOR_BLOCK || !AKKU_LADESTAND){ MOTOR_OFF;}
		else if(BETRIEB_TASTER && START_TASTER){
		        while(BETRIEB_TASTER && !ROTOR_BLOCK && AKKU_LADESTAND){ MOTOR_ON;}
		        MOTOR_OFF;}
	
	if(!AKKU_LADESTAND){ LEDS &= ~(1<<2)||(1<<1); blink();}
		else{ AKKU_LEER_OFF; LEDS = AKKU_LADESTAND;}

	WaitMilliseconds(100);
	
  } // endlos                               
} // main()

/*------------------------------- Funktionen ---------------------------------*/
void PositiveFlankenerkennung(void)
{
  static char TastenVorher = 0;	   // Beim ersten Aufruf mit Null initialisieren
  char TasteAktuell;

  TasteAktuell = TASTER;              // Tasten einlesen und den Wechsel von
  TastenAuswertung = TasteAktuell & ~TastenVorher;   // LOW auf HIGH erkennen
  TastenVorher = TasteAktuell;        // Aktuelles Muster in Erinnerung behalten
}

void WaitMilliseconds(int MilliSeconds)
{
  for(int i = 0; i < MilliSeconds; i++)
  {
    _delay_ms(1);
  }  
}

void blink(void){
	if(BlinkZaehler == 0) AKKU_LEER_ON;
	if(BlinkZaehler == ImpulsDauer) AKKU_LEER_OFF;
	if(BlinkZaehler == PeriodenDauer - 1) BlinkZaehler = 0;
	else BlinkZaehler++;
}