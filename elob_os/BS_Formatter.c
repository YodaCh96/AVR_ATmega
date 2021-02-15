/*
* BS_Formatter.c
*
* Created: 27.07.2018 16:37:39
*  Author: pst
*/ 

#include <avr/io.h>
#include <avr/pgmspace.h>   
#include "BS_Formatter.h"
#include "hd44780.h"


// function foreward declaration
const char* disp7Seg_ExecuteCommand(const char* pFrm); // try to execute for the 7Seg Display

const char* dummyCallBack(const char *pFrm)
{
   return pFrm;
}


// the function pointer for the call back function
typedef const char* (*fCallBack)(const char *pFrm);  /**< @brief CallBack function definition   */


fCallBack   fpCallBack = dummyCallBack;  /**< @brief contains the actual fCallBack() function   */


static struct frm_Functions  pf_Default = {lcd_putc, lcd_ExecuteCommand};
static struct frm_Functions  pf_7Seg    = { disp7Seg_put, disp7Seg_ExecuteCommand};
static struct frm_Functions  pf_Uart1   = {uart1_putc,    0 };

static struct frm_Functions  *_pFunc = &pf_Default;





/**< @brief contains the actual LcdStatus flags   */
static uint8_t LcdStatus = LCD_SET_DISPLAY | LCD_DISPLAY_ON | LCD_CURSOR_OFF | LCD_BLINKING_OFF;


// ============================================
// set the cursor (write position) to
// Col(0..15), Line=(0..3)
//
const char TblCmdSetCursor[] PROGMEM =
{
   LCD_DDADR_LINE1,
   LCD_DDADR_LINE2,
   LCD_DDADR_LINE3,
   LCD_DDADR_LINE4
};



void lcd_SetCursor(uint8_t Col, const uint8_t Line )
{
   lcd_command(Col + pgm_read_byte(&TblCmdSetCursor[Line]) + LCD_SET_DDADR);
}


void  lcd_WrDataRep(const char data, uint8_t Cnt)
{
   while(Cnt--)
     lcd_putc(data);
}



static void lcd_ClearLine(const char Line)
{
   lcd_SetCursor(0, Line);
   lcd_WrDataRep(' ', LCD_CHARCNT);
   lcd_SetCursor(0, Line);
}



const char* disp7Seg_ExecuteCommand(const char* pFrm) // try to execute for the 7Seg Display
{
   char C = *pFrm++;
   char Para0;

   switch(C) {
      case FRM_7SEG_CLEAR:               // clear the display
         disp7Seg_clear();
      break;

      case FRM_7SEG_SETDEC:              // 'd',n set decimal point at pos n starts with '0'
         Para0 = (*pFrm++) - '0';
         disp7Seg_setDecimal(Para0);
      break;

      case FRM_7SEG_SETCOLUMN:           // ':'   set the column symbol
         DISP5SEG_SET_COLON_ON();
      break;

      case FRM_7SEG_BLINKON:             // 'B'   set  blinking on                       +
         disp7Seg_setBlinkOnOff(ON);
      break;

      case FRM_7SEG_BLINKOFF:            // 'b'   set  blinking off                      +
         disp7Seg_setBlinkOnOff(OFF);
      break;

      case FRM_7SEG_WRTOBLINK:          // 'X'   the blink OFF will display this
         disp7Seg_setWriteToOff(ON);
      break;

      case FRM_7SEG_WRTOBLINKOFF:       // 'x'   
         disp7Seg_setWriteToOff(OFF);
      break;

      case FRM_7SEG_HOME:               // 'H'   go to home position                          +
         disp7Seg_setPos(4);
      break;
   }
   return pFrm;
}



const char* lcd_ExecuteCommand(const char* pFrm) // try to execute from the lcd
{
   char C = *pFrm++;
   char Para0;
   // here are all commands
   switch(C) {
      case FRM_CUR_ON:               // '+'   set cursor on
         LcdStatus |= LCD_CURSOR_ON;
         lcd_command(LcdStatus);
      break;

      case FRM_CUR_OFF:              // '-'   set cursor off
         LcdStatus &= ~LCD_CURSOR_ON;
         lcd_command(LcdStatus);
      break;

      case FRM_BLINKON:               // 'B'   set cursor blinking on
         LcdStatus |= LCD_BLINKING_ON;
         lcd_command(LcdStatus);
      break;

      case FRM_BLINKOFF:              // 'b'   set cursor blinking off
         LcdStatus &= ~LCD_BLINKING_ON;
         lcd_command(LcdStatus);
      break;

      case FRM_HOME:                  // 'H'   go to home position
         lcd_command(LCD_CURSOR_HOME);
      break;

      case FRM_LINE0:              // '0'   go to line 0 start position
         lcd_command(LCD_SET_DDADR + LCD_DDADR_LINE1);
      break;

      #if (LCD_DISPLAY_LINES >=1)
      case FRM_LINE1:              // '1'   go to line 1 start position
         lcd_command(LCD_SET_DDADR + LCD_DDADR_LINE2);
      break;
      #endif

      #if (LCD_DISPLAY_LINES >=2)
      case FRM_LINE2:                 // '2'   go to line 2 start position
         lcd_command(LCD_SET_DDADR + LCD_DDADR_LINE3);
      break;
      #endif

      #if (LCD_DISPLAY_LINES >=3)
      case FRM_LINE3:                 // '3'   go to line 3 start position
         lcd_command(LCD_SET_DDADR + LCD_DDADR_LINE4);
      break;
      #endif

      case FRM_CLEAR:
         lcd_command(LCD_CLEAR_DISPLAY); // pwr on
         tim_wait5mS(8);  // wait 40mS
      break;

      case FRM_DISPON:
         lcd_command(0b00010111);    // pwr on
         lcd_command(0b00010100);    // cur move, shift R
         lcd_command(0x08|0x04);     // lcd on
         LcdStatus |= LCD_DISPLAY_ON;
      break;

      case FRM_DISPOFF:
         lcd_command(0x08);          // lcd off
         lcd_command(0b00010011);
         lcd_command(0b00010100);
         LcdStatus &= ~LCD_DISPLAY_ON;
      break;

      case FRM_CLLINE0:
         lcd_ClearLine(0);               //
         lcd_command(LCD_SET_DDADR + LCD_DDADR_LINE1);
      break;

      #if (LCD_DISPLAY_LINES >=1)
      case FRM_CLLINE1: 
         lcd_ClearLine(1);
         lcd_command(LCD_SET_DDADR + LCD_DDADR_LINE2);
      break;
      #endif

      #if (LCD_DISPLAY_LINES >=2)
      case FRM_CLLINE2:
         lcd_ClearLine(2);
         lcd_command(LCD_SET_DDADR + LCD_DDADR_LINE3);
      break;
      #endif

      #if (LCD_DISPLAY_LINES >=3)
      case FRM_CLLINE3:
         lcd_ClearLine(3);
         lcd_command(LCD_SET_DDADR + LCD_DDADR_LINE4);
      break;
      #endif


      case FRM_SETPOS:    // 'P',x,y Set Position    x(line) and y(col) starts with '0'
         Para0 = *pFrm++;
         C     = *pFrm++;
         lcd_SetCursor(C-'0', Para0-'0');
      break;
   }
   return pFrm;
}


 
// pure string printing
void  frm_write(const char* pTxt)
{
   char C;
   while ((C= *pTxt++))
      _pFunc->pf_putc(C); // print the char
}


void frm_puts_P(const char* pFlashFrm)
{
   char    Buffer[64];

   strncpy_P(Buffer, pFlashFrm, 64);
   frm_puts(Buffer);
}


//extern fOutFunc    fPutOut;     /**< @brief = this is the actual output function   */
extern fCallBack   fpCallBack;  /**< @brief the callback function var for the  FRM_CALLBACK code   */


/**
   @brief   executes the pFrm and prints them to the given device
   @param   pFrm  the string to output
            Device where to output the pFrm
            DEVICE_LCD        
            DEVICE_DISP7SEG   
            DEVICE_UART1      

   @return  none
*/


void  frm_putsTo(const uint8_t Device)
{
   
   switch (Device) {
      case DEVICE_LCD:
         _pFunc = &pf_Default;  
      break;

      case DEVICE_DISP7SEG:
         _pFunc = &pf_7Seg;  
      break;

      case DEVICE_UART1:
         _pFunc = &pf_Uart1;
      break;

      default:
         _pFunc = &pf_7Seg;  
   }
}



// this function is not very helpful, but ....
void frm_put(uint8_t C)
{
   uint8_t Buff[2];
   Buff[0] = C;
   Buff[1] = 0;
   frm_puts((char*)Buff);
}

void frm_puts(const char* pFrm)
{
   uint8_t cnt;
   char    C;
   struct frm_Functions *pFuncBackUp = _pFunc;

    
   while( (C = *pFrm++) ) {
      if(C != FRM_CMD)        // is it not a command ? 
         _pFunc->pf_putc(C);  // then print
      else {
         C = *pFrm++;        // get command char
         switch(C) {         // over all commands
            case FRM_CMD:    // CMD,CMD print CMD
               _pFunc->pf_putc(C);
            break;

            case FRM_SPACE:                    // 'S',n print n space character  n starts with '0'
               if((C = *pFrm++) >= '0')        // is legal ?
                  lcd_WrDataRep(' ', C - '0'); // get the char to print rep
            break;

            case FRM_REPCHR:                   // 'R',n, chr print n times the chr n starts with '0'
               if((C = *pFrm++) >= '0') {      // is legal ?
                  cnt = C - '0';               // number to print chars
                  lcd_WrDataRep(*pFrm++, cnt); // get the char to print rep
               }
            break;

            case FRM_CRLF:                     // 'l'   prints cr,lf
               _pFunc->pf_putc(0x0D);
               _pFunc->pf_putc(0x0A);
            break; 

            case FRM_CALLBACK:                 // '>' call the callback function
               pFrm = fpCallBack(pFrm);
            break;             

            case FRM_Bell:                     // 'G' beep
               Buzzer(10);
            break;

            case FRM_DEVICE:                   // '<' redirect output
               C = *pFrm++;                   // get Device coding
               if(C == 'L')
                  _pFunc = &pf_Default;
               else if(C == 'U')
                  _pFunc = &pf_Uart1;
               else if(C == '7')
                  _pFunc = &pf_7Seg;
               else
                  _pFunc = &pf_Default;
            break;

            case FRM_WAIT:                   // 'W',n wait
               cnt = *pFrm++;
               if(cnt  >= '@') {
                  C  = 10;
                  cnt -= '@';
               }
               else {
                  C     =  1;
                  cnt -= '0';
               }
               while(cnt--) 
                  tim_wait5mS(C);  //
            break;
         } // end switch(C)

        // end of general implemented commands
        if (_pFunc->pf_Executer != 0) {
           pFrm = _pFunc->pf_Executer(pFrm-1); //  pf_putc(0x0D);
        }
     } // if (C != FRM_CMD)

     if(C == 0)
       break;
   } // end while

   _pFunc = pFuncBackUp;  // restore the default output
}



