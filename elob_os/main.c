/*******************************************************************************
Firma        : GIBZ
Autor        : Pius Steiner

Projekt      : Vorlagenprojekt Main zum Testen 
Version      : 1.0
Dateiname    : mainc.c
Erstelldatum : 02.08.2018

********************************************************************************
Chip type         : ATmega2560
CPU-Clock         : 16.0000 MHz (externer Quartz)
Programm Memory   : 256 kByte
Internal RAM      : 8 kByte
EEPROM            : 4 kByte
Data Stack size   : 16 Byte

********************************************************************************
Datum             Vers.    Kommentar / Änderungsgrund
02.08.2018        1.0      Erstellung

*******************************************************************************/




/*************************************************************************
Title:    ELOB Test Programm for the simple output driving library 
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
// #include <stdio.h>  // for NULL definition
#include "elob_base.h"




#define INIT_TESTPIN()   (DDRE |= (1<<PINE2))  // TestPin init
#define TOGGLE_TESTPIN() (PORTE ^=(1<<PINE2)) 

#define SIMU_RUN


/* 
   PORT-Names are moved to elob_base.h
*/


//Constant-Data


//Global Flags & Variables


char RxBuff[100];
uint8_t key;


static const __flash int value = 10;


static const __flash char FlashArr[] = "Ein Text den ich benötige ";


int main(void)
{
   uint8_t Event;
   INIT_TESTPIN();

//#ifndef SIMU_RUN
   elob_init();                  // initialize the hardware of ELOB 
//#endif


   frm_puts(FlashArr);                  // to 7Seg Display

   // the string contend (some escaping commands @) are documented in BS_Formatter.h
   frm_puts("@CELOB Test@5is started"); // to default (LCD)
   frm_puts("@<UHello ELOB started@l");  // to UART1
   frm_puts("@<7Hell");                  // to 7Seg Display




   // see elob_base.h

   // tool_NumFormat(34, "@CnnN.n", frm_puts);

   // ds1307_init();  // does not yet work

   uint8_t Irgendetwas = 0;

   adc_startConversionCumulate(ADC_POTI, 8);


   //adc_startConversion( ADC_POTI );

   while(1) {


      // read in the keys actual
      Event = deQueue();
      RGB_BLUEOFF() ;
      
      switch(Event) {
         case EVENT_KEY_PRESSED:
            key_getPressed();
//       disp7Seg_put(key);
//       disp7Seg_putNumber(key, 10);
         break;

         case EVENT_KEY_RELEASED:
            key = key_getReleased();
            if(key == '1')  {
               disp7Seg_clear();
               disp7Seg_put('1');
               disp7Seg_put('2');
               disp7Seg_put(',');
               disp7Seg_put('3');
               disp7Seg_put('4');
            }
            else if(key == '2')
               frm_puts("@<7@B");  // blink on
            else if(key == '3')  
               frm_puts("@<7@b");  // blink off
            else if(key == '7')
               frm_puts("@<7@X----@x");  
            else if(key == '8')
               frm_puts("@<7@X    @x");  
  /*          else if(key == '0')
               Buzzer(100);
               */
            key=0;  // done
         break;

         case EVENT_BUZZER_OFF:
            Irgendetwas++;
         break;

         case EVENT_TIM_SECONDE:
            //adc_startConversion( ADC_POTI );
          /*  RGB_BLUEON();*/
         break;

         case EVENT_ADC_FINISHED:
            disp7Seg_putNumber(adc_getValue(), 10);
            adc_startConversionCumulate(ADC_POTI, 16);            
//             RGB_REDON();
//             RGB_REDOFF() ;
         break;

         case EVENT_RXUART0:  // UART0 received data
         case EVENT_RXUART1:  // UART1 received data
            uart1_getToBuff(RxBuff);   // copy the received data tu RxBuff
            uart1_puts(RxBuff);        // Echo

            remove_atEndCRLF(RxBuff);  //
            frm_puts(RxBuff);  // output them 


         break;
      }
   }
}


