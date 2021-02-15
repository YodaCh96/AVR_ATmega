/*******************************************************************************
Firma        : Roche Diagnostics International AG
Autor        : Vorname Nachname

Projekt      : Vorlagenprojekt
Version      : 1.0
Dateiname    : Vorlage.c
Erstelldatum : 06.04.2018

********************************************************************************
Chip type         : ATmega2560
CPU-Clock         : 16.0000 MHz (externer Quartz)
Programm Memory   : 256 kByte
Internal RAM      : 8 kByte
EEPROM            : 4 kByte
Data Stack size   : 16 Byte

********************************************************************************
Datum             Vers.    Kommentar / Änderungsgrund
06.04.2018        1.0      Erstellung

*******************************************************************************/




/*************************************************************************
Title:    ELOB simple output driving library 
Author:   Pius Steiner <pius.steiner@gmx.ch>   
@file     $Id: elob_led.c, v 1.0 2018/07/08 18:33:32 pi Exp $
Software: AVR-GCC 4.x
Hardware: ELOB V1722 by S-TEC electronics AG, Zug
License:  GNU General Public License 
          
DESCRIPTION:
    
USAGE:
    refer to the header file elob_output.h for a description of the routines. 

NOTES:
                    
LICENSE:
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
                        
*************************************************************************/


#include <avr/io.h>

#include <avr/interrupt.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "BS_Fifo.h"
#include "BS_Ser.h"
#include "hd44780.h"
//#include "i2cmaster.h"  // not yet fished
#include "elob_base.h"


// forward declaration
uint8_t ELOB_startConversion(uint8_t ADC_Number);


// ---------------------------------------------------
// Variable implementation
//
static volatile uint8_t   _Flags;
#define   FLG_ADCBUSY      (0x01)   // if set then the ADC is in use
#define   FLG_7SEGON       (0x02)   // if set then the 7Seg is on
#define   FLG_7SEG_BLINK   (0x04)   // set if blinking is on     (7Seg display)
#define   FLG_7SEG_WRTOOFF (0x08)   // writing to the OFF memory (7Seg display)
#define   FLG_BLINK        (0x10)   // used to blinking  (7Seg display)

uint8_t           Seg7Arr[4];      // the 7 segment content
uint8_t           Seg7ArrOFF[4];   // the 7 segment blinking off content

uint8_t           _Seg7Blink;      // the 7 segment blinking counter
uint8_t           _Seg7Sel;        // the 7 segment selection register 0-3

static volatile uint8_t  _kbReleased;      // the last released key
static volatile uint8_t  _kbTimeReleased;  // and the time (max250) 
static volatile uint8_t  _kbPressed;       // the last pressed key
static volatile uint8_t  _tim_WaitTime;    // the counter for the wait time (used in lcd)
static          uint8_t  _disp7SegCol;     // stores the col pos for the disp7Seg_put()

static uint8_t  _kbLast[4];                // internal used by the key scan process
static uint8_t  _kbPress[4];
static uint8_t  _kbState[4];
static uint8_t  _kbCnt[4];


uint8_t  _AdCntSet;
volatile  uint8_t  _AdCnt;                  // the AD cumulate counter 
volatile  uint32_t _AdCumulate;             // to this var will be added the Adc values until _AdCnt is zero
uint16_t    _AdcVal;                       // the last read adc value 
uint8_t     _BuzzerTime;                   // the time how long the buzzer should cry
uint8_t     _cntSecond;                    // down counter for the second event
uint8_t     Fifo[16];                      // the event queue
// ---------------------------------------------------





/* -------------------------------------------------------------------------- */
/**@brief  setup the output function
 */
 void  elob_init(void)
{
   _Flags = 0;   // reset all flags

   // setup port for the LED's
   ELOB_PORT_DDRLED = 0xFF;	// LEDs as Output
   ELOB_PORT_LED    = 0;      // switch all LED OFF 

   // setup the RGB LED port pins direction as output
   ELOB_PORT_DDRRGB |= ELOB_RGB_RED | ELOB_RGB_GREEN | ELOB_RGB_BLUE;

   // RGB LED all  OFF
   ELOB_PORT_RGB    &= ~(ELOB_RGB_RED | ELOB_RGB_GREEN | ELOB_RGB_BLUE);

   // ----------------------------------------------
   // set Port for the 7Seg Display
   SEG7_SELDDR |= (1<<SEG7_COLON) | (1<<SEG7_GRAD) | (1<<SEG7_EN) | (1<<SEG7_1) | (1<<SEG7_0);   // for output 
   _Seg7Sel    = 0;
   disp7Seg_clear();      // Clear them first
   
      // and the digit output (data)   
   SEG7_DIGIDDR  |= 0xff; // all out
   SEG7_DIGIPORT  = 0;    // 
   disp7Seg_setOnOff(ON);
   // ----------------------------------------------


   // ----------------------------------------------
   // set Port for the multiplexed keyboard 
   KEYB_INPORTDDR &= ~KEYB_INMASK; // as input
   _kbReleased     = _kbTimeReleased = _kbPressed = 0;
   // ----------------------------------------------

   fifo_init(Fifo,sizeof(Fifo));  // initialize the queue

   _cntSecond = 200;  // need for the second event 

   
   // setup the Timer5
   cli();

   // -------------------------------------------------------------------------
   // Start System-Clock Timer (TCNT 5)
   // is used as a general timer. 
   TCCR5A = 0b00000000;	  // CS div8 -> 1Clk = 4us, no PWM, Clear on Compare Match
   TCCR5B = (1<<WGM52)  | (1<<CS51) | (1<<CS50);
   TIMSK5 = (1<<OCIE5A);  // Output Compare Interrupt enable
   OCR5A  =  1250;		  // is 5mS @ 16MHz
   _tim_WaitTime = 0;     // the counter for the wait time

   // -------------------------------------------------------------------------


   // ----------------------------------------------------------------------
   // The RGB LED's are connected to PB5-PB7 or better OC1A -OC1C 
   // This makes them very easy to control by the PWM

   // Fast PWM mode 
   // (1<<WGM10) | (1<<WGM12)  (WGM11 WGM13) are 0
   // (1<<CS11)  prescaler clk/8
   // RGB Init (TCNT1)
   //       Fast PWM 8-bit mode   |   compare out mode for channel A B and C non inverting
   //  a value of 0 will not switch the led total dark. Therefor we switch the PWM out on setting the values
   TCCR1A = (1<<WGM10); //        | (1<<COM1A1) | (1<<COM1B1) | (1<<COM1C1);

   //   prescaler clk/8  and rest of mode
   TCCR1B = (1<<CS11)  | (1<<WGM12);

   // no interrupt used for this timer 
   TIMSK1 = 0b00000000;

   OCR1A = 0; // this is the red 
   OCR1B = 0; // this is the green
   OCR1C = 0; // and the blue 
   // end of RGB LED's -----------------------------------------------------


   // ----------------------------------------------------------------------
   // ADC initialization
   // ----------------------------------------------------------------------

   // the R trimmer is connected to ADC3/PF3 pin

   _AdCntSet   = 0;    // how many times to cumulate
   _AdCnt      = 0;    // the AD cumulate counter
   _AdCumulate = 0;    // the cumulate sum register
   ADMUX  = 0b00000000;  // internal Vref turned off, right adjust

   // Div /4 is to fast
   // Div /8 is to fast Error +-2
   // Div /16 is ok  
   // Div /32 is ok  
   // Div /64 is ok  
   ADCSRA = (1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (1<<ADIF) | (1<<ADIE) | (0<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
   ADCSRB = 0x00;

   // ----------------------------------------------------------------------

   sei();	

   lcd_init();          // initializes the lcd display

   uart1_init( UART_BAUD_SELECT(UART1_BAUD_RATE ,F_CPU) ); // // initializes the UART1 (USB interface)
}


uint8_t deQueue(void)
{
   uint8_t Event = 0;
   return ( fifo_get(Fifo, &Event) ) ? 0 : Event;
}

uint8_t putQueue(uint8_t Event)
{
   return fifo_put(Fifo, Event);
}




/*
   ----------------------------------------------------------
   Implementation of the AD Converter
   ----------------------------------------------------------
*/


uint8_t adc_startConversionCumulate(uint8_t ADC_Number, uint8_t No)
{
   if(!(_Flags & FLG_ADCBUSY)) {    // start only if the ADC is not in conversion mode
      _AdCnt =_AdCntSet = No;       // the AD cumulate counter
      _AdCumulate = 0;              // reset the cumulation register
      return adc_startConversion(ADC_Number); // now start the conversion
   }
   return RET_NOTOK;
}


// fuers erste mal zum testen
uint8_t adc_startConversion(uint8_t ADC_Number)
{
   if(!(_Flags & FLG_ADCBUSY)) {  // start only if the ADC is not in conversion mode
      if(ADC_Number < 8) {
         ADCSRB &= ~0x08;        // clear MUX5
         ADMUX  &= ~0x1F;        // clear MUX0-4
         ADMUX  += ADC_Number;   // set MUX0-4 according to ADC_Number
      } 
      else {
         ADCSRB |= 0x08;         // set MUX5
         ADMUX  &= ~0x1F;        // clear MUX0-4
         ADMUX  += ADC_Number-8; // set MUX0-4 according to ADC_Number
      }
      _Flags |= FLG_ADCBUSY;     // set busy
      ADCSRA |= (1<<ADSC);       // start conversion
      return RET_OK;
   }
   return RET_NOTOK;
}


uint16_t adc_getValue(void)
{
   uint16_t  RetVal;
   uint8_t   CopyOfSREG = SREG; // save global interrupt flags
   cli();                       // block now all interrupts

   RetVal = _AdcVal;             // get the value
   SREG = CopyOfSREG;           // restore the interrupts

   return RetVal;
}

/*
   ----------------------------------------------------------
   Implementation of the 7Seg 4digit display of the board
   ----------------------------------------------------------
   Table to convert bin numbers to 7-seg digits
         1
       -----
    32 | 64| 2
       -----
    16 |   | 4
       -----
         8   . 128
*/
const char TBL_7SegDef[] PROGMEM = {
   '0', 0x3F,
   '1', 0x06,
   '2', 0x5B,
   '3', 0x4F,
   '4', 0x66,
   '5', 0x6D,
   '6', 0x7D,
   '7', 0x07,
   '8', 0x7F,
   '9', 0x6F,
   '-', 0x40,
   'A', 0x77,
   'B', 0x7C,
   'C', 0x39,
   'D', 0x5E,
   'E', 0x79,
   'F', 0x71,
   'H', 2+4+16+32+64,
   'R', 0x50,
   'L', 32 + 16 + 8,
   'O', 4 + 16 + 8 + 64,
   ' ', 0,
     0, 0
};	// lookup table 




void disp7Seg_setOnOff(uint8_t On)
{
   if(On)
      _Flags |= FLG_7SEGON;
   else
      _Flags &= ~FLG_7SEGON;   
}

void disp7Seg_setBlinkOnOff(uint8_t On)
{
   if(On)
      _Flags |= FLG_7SEG_BLINK;
   else
      _Flags &= ~FLG_7SEG_BLINK;
}




void disp7Seg_clear(void)
{
   _disp7SegCol = 4;

   Seg7Arr[0] = Seg7Arr[1] = Seg7Arr[2] = Seg7Arr[3] = 0;
   DISP5SEG_SET_GRAD_OFF();
   DISP5SEG_SET_COLON_OFF();
}



/**
   @brief   print a number to the 7 seg Display
   @param   Val   the number to print
   @param   Radix 
   @return  the length of the converted result 
*/
uint8_t disp7Seg_putNumber(int16_t Val, uint8_t Radix)
{
   char Buff[12];
   uint8_t  Len, Col = 0;

   disp7Seg_clear();

   //_disp7SegCol = 4;
   //Seg7Arr[0] = Seg7Arr[1] = Seg7Arr[2] = Seg7Arr[3] = 0;


   
   itoa ( Val, Buff, Radix );  // convert the number to ascii
   Len = strlen(Buff)-1;       // how big 

   TIMSK5 &= ~(1<<OCIE5A);  // Output Compare Interrupt enable
// cli();
   do  { // fill from right to left the converted char into the display memory 
      Seg7Arr[Col]   &= ~127; // all off except dec point
      Seg7Arr[Col++] |= tool_lookupByte_P( toupper( Buff[Len] ) , TBL_7SegDef);
   }
   while(Len--);
   TIMSK5 = (1<<OCIE5A);  // Output Compare Interrupt enable

// sei();
   return strlen(Buff);
}

//volatile  uint8_t Seg7Arr[4];
void disp7Seg_setDecimal(int8_t Val)
{
   if(Val < 4)
      Seg7Arr[Val] |= 128;
}

void disp7Seg_clearDecimal(int8_t Val)
{
   if(Val < 4)
      Seg7Arr[Val] &= ~128;
}

void disp7Seg_setWriteToOff(uint8_t On)
{
   if(On)
      _Flags |= FLG_7SEG_WRTOOFF;
   else
      _Flags &= ~FLG_7SEG_WRTOOFF;
}

void disp7Seg_setPos(uint8_t Pos)
{
   _disp7SegCol = Pos;
}




void disp7Seg_put(const char C)
{
   char *pDest = (_Flags & FLG_7SEG_WRTOOFF) ? (char*)&Seg7ArrOFF : (char*)&Seg7Arr;

   if(_disp7SegCol <= 4) {
      switch (C) {
         case '.':                         // is it a decimal point ?
         case ',':
            *(pDest+_disp7SegCol) |= 128;  // set the decimal point without changing the col pos
         break;
         case ':':                         // print : only on Pos1
            if(_disp7SegCol == 2)
               DISP5SEG_SET_COLON_ON();
         break;
         default:
            *(pDest+ --_disp7SegCol) = tool_lookupByte_P( toupper( C ) , TBL_7SegDef);
      }
   }
   if(_disp7SegCol == 0)
      _disp7SegCol = 4;   // back to most left digit
}




/*
   ----------------------------------------------------------
   Implementation of the RGB led
   ----------------------------------------------------------

*/   

void  rgb_setRed(const uint8_t red)
{
   if(red) {
      OCR1A   = red;
      TCCR1A |= (1<<COM1A1); // 
   }
   else
      TCCR1A &= ~(1<<COM1A1); //
}

void  rgb_setGreen(const uint8_t green)
{
   if(green) {
      OCR1B   = green;
      TCCR1A |= (1<<COM1B1);
   }
   else
   TCCR1A &= ~(1<<COM1B1); //
}

void  rgb_setBlue(const uint8_t blue)
{
   if(blue) {
      OCR1C   = blue;
      TCCR1A |= (1<<COM1C1);
   }
   else
      TCCR1A &= ~(1<<COM1C1); //
}

void  rgb_set(const uint8_t red, const uint8_t green, const uint8_t blue)
{
   rgb_setRed(red);
   rgb_setGreen(green);
   rgb_setBlue(blue);
}



/*
   ----------------------------------------------------------
   Implementation of the buzzer
   ----------------------------------------------------------
*/

void  Buzzer(uint8_t Time) 
{
   _BuzzerTime = Time;
   BUZZER_ON;
}


const char TBL_Keyboard[] PROGMEM = {
    0x12, '0',
    0x21, '1',
    0x22, '2',
    0x24, '3',
    0x31, '4',
    0x32, '5',
    0x34, '6',
    0x01, '7',
    0x02, '8',
    0x04, '9',
    0x09, 'A',
    0x12, 'B',
     0, 0
};	// lookup table




uint8_t key_getPressed(void)
{
   uint8_t Key = 0;
   if(_kbPressed) {  // is a key ready ?
      Key = tool_lookupByte_P(_kbPressed, TBL_Keyboard);
      _kbPressed = 0; // clear them now
   }
   return Key;
}

uint8_t key_getReleased(void)
{
   uint8_t Key = 0;
   if(_kbReleased) {  // is a key ready ?
      Key = tool_lookupByte_P(_kbReleased, TBL_Keyboard);
      _kbReleased = 0; // clear them now
   }
   return Key;
}


#define KB_PRESS_TIME   2   // how many counts until key pressed is detected
#define KB_STATE_IDLE   0
#define KB_STATE_PRESS  1
#define KB_STATE_PRESS2 2


// is called each 5mS. That is enough for a non flickering 7-seg display
// to scan the keyboard and diverse slow time timers
ISR(TIMER5_COMPA_vect)
{
   uint8_t Col = _Seg7Sel & 0x03; // set the col
   uint8_t K = 0;

   // select the col selection (7-seg display and keyboard) 
   SEG7_SELPORT &= ~((1<<SEG7_0) | (1<<SEG7_1) | (1<<SEG7_EN)); // col off
   SEG7_SELPORT |= Col;// | (1<<SEG7_EN);

   //   Seg7Blink
   // set 7-seg display data if it is on

   if(_Flags & FLG_7SEGON) {            // is 7Seg Display on ?
      if(_Flags & FLG_7SEG_BLINK)       // is blinking on?
         SEG7_DIGIPORT = (_Flags & FLG_BLINK) ? Seg7Arr[Col] : Seg7ArrOFF[Col];
      else
         SEG7_DIGIPORT = Seg7Arr[Col];
   }
   else  // nothing to display
      SEG7_DIGIPORT = 0;


   SEG7_SELPORT |= Col;// | (1<<SEG7_EN);

   // the 7Seg and the keyboard Col is now selected
   // Col contains the selected number

   // ----------------------------------------------------

   // read in the actual keys

   SEG7_SELPORT |= (1<<SEG7_EN);
   K = ((PIND & KEYB_INMASK) >> KEYB_INMASKSHIFT);
   K  = (K) ? (K | (Col<<4)): 0;

   SEG7_SELPORT &= ~(1<<SEG7_EN);
   SEG7_SELPORT |= (1<<SEG7_EN);

   switch(_kbState[Col]) {
      case KB_STATE_IDLE:    // no key pressed
         _kbLast[Col] = _kbPress[Col] = _kbCnt[Col] = 0;
         if(K) {
            _kbLast[Col]  = K;
            _kbState[Col] = KB_STATE_PRESS;
         }
      break;

      case KB_STATE_PRESS:    // key is now pressed
         if(K) 
            _kbState[Col] = (K == _kbLast[Col]) ? KB_STATE_PRESS2 : KB_STATE_IDLE;
         else
            _kbState[Col] = KB_STATE_IDLE;
      break;

      case KB_STATE_PRESS2:   // still the same key is pressed  
         if(K == _kbLast[Col]) {
            _kbPress[Col] = K;
            if(++_kbCnt[Col] == 251)  // count time and blocking overrun
               _kbCnt[Col] = 250;     // limit
            if (_kbCnt[Col] == KB_PRESS_TIME) {
               _kbPressed = _kbLast[Col];
               fifo_put(Fifo, EVENT_KEY_PRESSED);
            }
         }
         else {               // is now more the same or released
            if(_kbCnt[Col] >= KB_PRESS_TIME ) {
               _kbReleased     = _kbLast[Col];
               _kbTimeReleased = _kbCnt[Col];
               fifo_put(Fifo, EVENT_KEY_RELEASED);
            }
            _kbState[Col]   = KB_STATE_IDLE;
         }
      break;

      default:
         _kbState[Col] = KB_STATE_IDLE;
   }
   // ----------------------------------------------------
         
   --_Seg7Blink;
   if(_Seg7Blink > BLINK_VAL)
      _Flags |= FLG_BLINK;
   else
      _Flags &= ~FLG_BLINK;

   if(_BuzzerTime) {
      --_BuzzerTime;
      if(!_BuzzerTime) {
         BUZZER_OFF;
         fifo_put(Fifo, EVENT_BUZZER_OFF);
      }
   }

   if(!(--_cntSecond)) {
      fifo_put(Fifo, EVENT_TIM_SECONDE);
      _cntSecond = 200;
   }

   if(_tim_WaitTime) 
      --_tim_WaitTime;
   
   ++_Seg7Sel;  // next col 
}









/* -------------------------------------------------------------------------- */
/**@brief  ADC conversion ready interrupt 
 */
/*
    _AdCntSet   = 0;    // how many times to cumulate
    _AdCnt      = 0;    // the AD cumulate counter
    _AdCumulate = 0;    // the cumulate sum register
*/
ISR(ADC_vect)
{
   if(_AdCntSet) {  // cumulating AD values?
     _AdCumulate += ADC;
     if(!(--_AdCnt)) {  // cumulating at end?
        _AdCumulate = _AdCumulate / (_AdCntSet); // calculate average 
        _AdcVal = (uint16_t) _AdCumulate; // & 0x3FF;
        fifo_put(Fifo, EVENT_ADC_FINISHED);
       _Flags &= ~FLG_ADCBUSY;      // no more busy
       _AdCntSet = 0;
     }
     else {  // start AD again
        ADCSRA |= (1<<ADSC);       // start conversion
     }
   }
   else {
      _AdcVal = 0; // TEST TEST ADC;
      fifo_put(Fifo, EVENT_ADC_FINISHED);
      _Flags &= ~FLG_ADCBUSY;      // no more busy
   }
}




inline void  tim_wait5mS(const int8_t Time)
{
   _tim_WaitTime = Time;
   while(_tim_WaitTime) {
      ;
   }
}



// ------------------------------------------------------------------
// Tool functions 
// ------------------------------------------------------------------
// 
/**
   @brief   searches a byte in progmem table
   @param   <tt>C</tt> to search 
            <tt>pTable</tt>  the pointer to the table. The table has to bee terminated by a \0
   @return  uint8_t the index where the byte was found or <tt>RET_IDXFAIL</tt> (0xff) if the byte is not found

   The tables first char is the searching char and the second is the returned value
   @details
   const char TBL_7SegDef[] PROGMEM = {
      '0', 0x3F,
      '1', 0x06,
      '2', 0x5B,
      '3', 0x4F,
      \0;
   }
*/
uint8_t tool_lookupByte_P(const int8_t C, const char* PROGMEM pTable)
{
   uint8_t c;
   uint8_t Idx = 0;

   do {
      if((c = pgm_read_byte( pTable + Idx++)) == C)
         return pgm_read_byte( pTable + Idx);
      ++Idx;
   } while (c);    
   
   return RET_IDXFAIL;
}

/**
   @brief   searches at the end of the pBuff for \r\n and remove them, as long as 
            the \r\n are at the end of the buffer
   @param   pBuff Buffer to process

   @return  returns the length of the result buffer
*/

uint8_t remove_atEndCRLF(char* pBuff)
{
   if(pBuff) {
      uint8_t Len = strlen(pBuff);
      pBuff += Len-1;  // last char 
      while(*pBuff == 13 || *pBuff == 10) {
         *pBuff-- = 0; 
         --Len;
      }
      return Len;
   }
   return 0; 
}

// dummy function at now
void error_put(const uint8_t ErrCode)
{
   // mach etwas
}


//
// Converts a number into a simple string and write them to FOutC
//
//  PIUS die Funktion muss man noch sauber machen!!
// -nnnN.nnn
// + means print sign + or -
// - means print only -

int8_t tool_NumFormat(const int32_t Num, const char* pFormat,  fOutFunc FOutC)
{
   static uint8_t NumFormatRadix = 10;

   char   Buff[12];
   char   BuffResult[32];
   char   c;
   char*  pD;
   int8_t IdxS,IdxD, NumLen, FormatLen = 0;

   char* pStart = BuffResult; 

   if(!pFormat)
      ltoa(Num, pStart, NumFormatRadix);
   else {
      ltoa(Num, Buff, NumFormatRadix);

      FormatLen = strlen(pFormat);
      NumLen    = strlen(Buff);

      if(NumLen > FormatLen)  // is a error
         memcpy(pStart, Buff, NumLen+1);
      else {
         memcpy(pStart, pFormat, FormatLen+1); // +1 cpy also \0

         IdxS   = NumLen-1;
         IdxD   = FormatLen-1;

         while(IdxD >=0) { 
            pD = &BuffResult[IdxD];
            c =  *pD;

            if(c == 'N' || c=='n') {   // is it a digit format
               if(IdxS >= 0) {
                  *pD = (Buff[IdxS]== '-') ? ' ': Buff[IdxS--];
               }
               else {
                  *pD = (c=='N') ? '0': ' ';
               }
            }
            else if(c == '+')    // show both  +-
               *pD = (Num<0) ? '-': '+';
            else if(c == '-')   // show only -
               *pD = (Num<0) ? '-': ' ';
            
            --IdxD;  // next frm char
         }
      }
   }
   frm_puts(BuffResult);
   return strlen(BuffResult);
}
