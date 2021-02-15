/************************************************************************
Title:    
Author:   Pius Steiner  <pius.steiner@gmx.ch>  
File:     $Id: BS_Formatter.h, v 1.0 2018  $
Software: AVR-GCC 4.x, AVR Libc 1.4 or higher
Hardware: ELOB Board ATMega2560 (tested)
Usage:    see Doxygen manual

LICENSE:
    Copyright (C) 2018 Pius Steiner, GNU General Public License Version 3

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
************************************************************************/

/*******************************************************************************
Firma        : GIBZ
Autor        : Pius Steiner <pius.steiner@gmx.ch> 

Projekt      : Vorlage
Version      : 0.9
Dateiname    : elob_output.h
Erstelldatum : 2018 July 

********************************************************************************
Chip type         : ATmega2560
CPU-Clock         : 16.0000 MHz (external Crystal)
Programm Memory   : 256 kByte
Internal RAM      : 8 kByte
EEPROM            : 4 kByte
Data Stack size   : 16 Byte

********************************************************************************
Datum             Vers.    Kommentar / Änderungsgrund
06.07.2018        0.9      erste Versuche

*******************************************************************************/


/** 
 *  
 *  @file
 *  @defgroup Formatter Formatter BS_Formatter.h
 *  @code #include "BS_Formatter.h" @endcode
 *
 *  @brief  Escaping string commands interpretation
 *
 *  Detailed description starts here.
 *
 *
 *  @note 
 *  @author Pius Steiner <pius.steiner@gibz.ch> 
 *  @copyright (C) 2018 Pius Steiner, GNU General Public License Version 3
 */

 
#ifndef BS_FORMATTER_H_
   #define BS_FORMATTER_H_

#include <avr/io.h>
#include "elob_base.h"


// the function pointer for the format function output
typedef const char* (*fCallBack)(const char *pFrm);
typedef const char* (*fExecuter)(const char *pFrm);
typedef void        (*fOutFunc)(const char c);

/**
 *  @brief   this structure contains two function pointer to service the actual output module
*/
struct frm_Functions {
   fOutFunc  pf_putc;       /*!< the char output function */
   fExecuter pf_Executer;   /*!< the device depended execute function interpreter for the output specific commands*/
};

// ==============================================
// The defined devices
// ==============================================

#define DEVICE_LCD        0x01 /*!< LCD display output */
#define DEVICE_DISP7SEG   0x02 /*!< 7 Seg display output */ 
#define DEVICE_UART1      0x03 /*!< UART1 output */
   

// ==============================================
// string command definitions
// ==============================================


/**
 *  @brief   These macros are used to determine the functions in text strings.

    The  function is used to specify commands directly in the output text, for example to position the 
    cursor or to delete the display. 
    Commands are marked with a reserved character, followed by a command character and any parameters.
    Example:
    The sequence "@CText" erases the whole LCD display and writes "Text" to the beginning of the LCD display.
    The '@' is the escape character and the 'C' is the command to delete the entire display. 
*/

#define FRM_CMD                '@'   /*!< command start (to print a @ write @@ )  */

/**
 *  @brief   general usable commands 
*/
#define FRM_SPACE              'S'   /*!< 'S',n print n spac@e character    n starts with '0' + */
#define FRM_REPCHR             'R'   /*!< 'R',n, chr print n times the chr n starts with '0' + */
#define FRM_WAIT               'W'   /*!< 'Y',x for x are the following wait times available */
#define FRM_CRLF               'l'   /*!< 'l'   prints cr,lf */
#define FRM_Bell               'G'   /*!< 'G' beep                                           + */
#define FRM_CALLBACK           '>'   /*!< '>' call the callback function                     + */
#define FRM_DEVICE             '<'   /*!< '<',D redirect the output function for D = 
                                            = L (LCD) W2                                   = U (UART1) 
                                            = 7 (7SegDisp) 
                                            = D (default here LCD)  */
/**
 *  @brief   command definition ONLY used for the LCD Display
*/
#define FRM_HOME               'H'   /*!< 'H'   go to home position                          + */
#define FRM_CLEAR              'C'   /*!< 'C'   clear display                                + */
#define FRM_CUR_ON             '+'   /*!< '+'   set cursor on                                + */
#define FRM_CUR_OFF            '-'   /*!< '-'   set cursor off                               + */
#define FRM_BLINKON            'B'   /*!< 'B'   set cursor blinking on                       + */
#define FRM_BLINKOFF           'b'   /*!< 'b'   set cursor blinking off                      + */

#define FRM_LINE0              '0'   /*!< '0'   go to line 0 start position                  + */
#define FRM_LINE1              '1'   /*!< '1'   go to line 1 start position                  + */
#define FRM_LINE2              '2'   /*!< '2'   go to line 2 start position */
#define FRM_LINE3              '3'   /*!< '3'   go to line 3 start position */
#define FRM_CLLINE0            '4'   /*!< '4'   clear line 0 and go to line 0 start position + */
#define FRM_CLLINE1            '5'   /*!< '5'   clear line 1 and go to line 1 start position + */
#define FRM_CLLINE2            '6'   /*!< '6'   clear line 2 and go to line 2 start position */
#define FRM_CLLINE3            '7'   /*!< '7'   clear line 3 and go to line 3 start position */
#define FRM_DISPON             'o'   /*!< 'o'   display on                                   + */
#define FRM_DISPOFF            'O'   /*!< 'O'   display off                                  + */
#define FRM_SETPOS             'P'   /*!< 'P',x,y Set Position    x(line) and y(col) starts with '0'  + */


/**
*   @brief   command definition ONLY used for the 7Seg Display
*/
#define FRM_7SEG_CLEAR         'C'   /*! 'C'   clear display                  */
#define FRM_7SEG_HOME          'H'   /*! 'H'   go to home position            */
#define FRM_7SEG_SETDEC        'd'   /*! 'd',n set decimal point at pos n starts with '0'*/
#define FRM_7SEG_SETCOLUMN     ':'   /*! ':'   set the column symbol          */
#define FRM_7SEG_BLINKON       'B'   /*! 'B'   set  blinking on               */
#define FRM_7SEG_BLINKOFF      'b'   /*! 'b'   set  blinking off              */
#define FRM_7SEG_WRTOBLINK     'X'   /*! 'X'   write to the OFF memory        */
#define FRM_7SEG_WRTOBLINKOFF  'x'   /*! 'x'   write to the normal memory     */



/*
   Variable definitions
*/

/**
 *   @brief   hold the actual output function definition 
*/
extern fCallBack   fpCallBack;  /**< @brief the callback function var for the  FRM_CALLBACK code   */


/**
 *   @brief   sets the output function definition 
 *   @param   pFlashFrm  the string to output
 *   @return  the previous defined function ptr 
*/
//extern fOutFunc frm_setOutFunc(fOutFunc pFunction);


/**
 *   @brief   executes the pFrm and prints them out
 *   @param   pFlashFrm  the string to output
 *   @return  none
*/
extern void    frm_puts_P(const char* pFlashFrm);


/**
 *   @brief   executes the pFrm and prints them out
 *   @param   pFrm  the string to output
 *   @return  none
*/
extern void    frm_puts(const char* pFrm);

/**
 *   @brief   executes the pFrm and prints them to the given device
 *   @param   Device where to output the pFrm
 *            DEVICE_LCD        
 *            DEVICE_DISP7SEG   
 *            DEVICE_UART1      
 *
 *   @return  none
*/
 extern void    frm_putsTo(const uint8_t Device);





// this function is not very helpful, but ....
extern void frm_put(uint8_t C);


/**
 *  @brief   pure writing function 
 *  @param   pTxt to write (\0 terminated)
 *  @return  none
*/
extern void   frm_write(const char* pTxt);



#endif /* BS_FORMATTER_H_ */