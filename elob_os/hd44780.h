/*****************************************************************************
Title  :   HD44780 Library
Author :   SA Development
Version:   1.11
*****************************************************************************/

#ifndef HD44780_H
   #define HD44780_H

#include "hd44780_settings.h"


/** 
 *  @file
 *  @defgroup ELOB_LCD  <hd44780.h>
 *  @code #include <hd44780.h> @endcode
 * 
 *  @brief 
 *
 *  
 *
 *  @note Based on 
 *  @author Pius Steiner  pius.steiner@gibz.ch
 *  @copyright (C) 2018  GNU General Public License Version 3
 */
 
 

// ==============================================
// LCD Commands and Arguments.
// zur Verwendung in lcd_command
// ==============================================

// ========================================================================
// Zeilendefinitionen des verwendeten LCD
// die Einträge hier sollten für ein LCD mit einer Zeilenlänge von 16 Zeichen passen
// bei anderen Zeilenlängen müssen diese Einträge angepasst werden
// ========================================================================

#define LCD_CHARCNT             (16)  // number of character per line
#define LCD_DDADR_LINE1         0x00
#define LCD_DDADR_LINE2         0x40
#define LCD_DDADR_LINE3         0x10
#define LCD_DDADR_LINE4         0x50


// Clear Display -------------- 0b00000001
#define LCD_CLEAR_DISPLAY       0x01

// Cursor Home ---------------- 0b0000001x
#define LCD_CURSOR_HOME         0x02

// Set Entry Mode ------------- 0b000001xx
#define LCD_SET_ENTRY           0x04

#define LCD_ENTRY_DECREASE      0x00
#define LCD_ENTRY_INCREASE      0x02
#define LCD_ENTRY_NOSHIFT       0x00
#define LCD_ENTRY_SHIFT         0x01

// Set Display ---------------- 0b00001xxx
#define LCD_SET_DISPLAY         0x08

#define LCD_DISPLAY_OFF         0x00
#define LCD_DISPLAY_ON          0x04
#define LCD_CURSOR_OFF          0x00
#define LCD_CURSOR_ON           0x02
#define LCD_BLINKING_OFF        0x00
#define LCD_BLINKING_ON         0x01

// Set Shift ------------------ 0b0001xxxx
#define LCD_SET_SHIFT           0x10

#define LCD_CURSOR_MOVE         0x00
#define LCD_DISPLAY_SHIFT       0x08
#define LCD_SHIFT_LEFT          0x00
#define LCD_SHIFT_RIGHT         0x01

// Set Function --------------- 0b001xxxxx
#define LCD_SET_FUNCTION        0x20

#define LCD_FUNCTION_4BIT       0x00
#define LCD_FUNCTION_8BIT       0x10
#define LCD_FUNCTION_1LINE      0x00
#define LCD_FUNCTION_2LINE      0x08
#define LCD_FUNCTION_5X7        0x00
#define LCD_FUNCTION_5X10       0x04

#define LCD_SOFT_RESET          0x30


// Set DD RAM Address --------- 0b1xxxxxxx  (Display Data RAM)
#define LCD_SET_DDADR           0x80

























//LCD Constants for HD44780
#define LCD_CLR                 0    // DB0: clear display

#define LCD_HOME                1    // DB1: return to home position

#define LCD_ENTRY_MODE          2    // DB2: set entry mode
#define LCD_ENTRY_INC           1    // DB1: 1=increment, 0=decrement
//#define LCD_ENTRY_SHIFT         0    // DB0: 1=display shift on

#define LCD_DISPLAYMODE         3    // DB3: turn lcd/cursor on
#define LCD_DISPLAYMODE_ON      2    // DB2: turn display on
#define LCD_DISPLAYMODE_CURSOR  1    // DB1: turn cursor on
#define LCD_DISPLAYMODE_BLINK   0    // DB0: blinking cursor

#define LCD_MOVE                4    // DB4: move cursor/display
#define LCD_MOVE_DISP           3    // DB3: move display (0-> cursor)
#define LCD_MOVE_RIGHT          2    // DB2: move right (0-> left)

#define LCD_FUNCTION            5    // DB5: function set
//#define LCD_FUNCTION_8BIT       4    // DB4: set 8BIT mode (0->4BIT mode)
#define LCD_FUNCTION_2LINES     3    // DB3: two lines (0->one line)
#define LCD_FUNCTION_10DOTS     2    // DB2: 5x10 font (0->5x7 font)

#define LCD_CGRAM               6    // DB6: set CG RAM address
#define LCD_DDRAM               7    // DB7: set DD RAM address

#define LCD_BUSY                7    // DB7: LCD is busy


const char* lcd_ExecuteCommand(const char* pFrm); // try to execute from the lcd

void lcd_init();
void lcd_command(uint8_t cmd);

void lcd_clrscr();
void lcd_home();
void lcd_goto(uint8_t pos);

#if RW_LINE_IMPLEMENTED==1
uint8_t lcd_getc();
#endif

void lcd_putc(char c);
void lcd_puts(const char *s);
void lcd_puts_P(const char *progmem_s);
#if (LCD_DISPLAYS>1)
void lcd_use_display(int ADisplay);
#endif

#endif

