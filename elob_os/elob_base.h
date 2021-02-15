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
 *  @mainpage ELOB Project
 *  @file
 *  @defgroup elob_base ELOB App elob_base.h
 *  @code #include <elob_base.h> @endcode
 *
 *  @brief A simple approach for an operating system
 *             on the developer board of the GIBZ Zug
 *
 *  Detailed description starts here.
 *
 *
 *  @note 
 *  @author Pius Steiner <pius.steiner@gmx.ch> 
 *  @copyright (C) 2018 Pius Steiner, GNU General Public License Version 3
 */

 
 
#ifndef ELOB_OUTPUT_H_
   #define ELOB_OUTPUT_H_


#include <avr/pgmspace.h>   
#include "bs_ser.h"
#include "bs_formatter.h"




// --------------------------------------------------------
// Physical definition
// --------------------------------------------------------

// 8 Leds are on this port connected

#define  ELOB_PORT_LED     PORTA  // The LEDs are connected to this port
#define  ELOB_PORT_DDRLED  DDRA   // and this is the direction register to the LED port
#define  ELOB_PORT_DDRLED  DDRA   // and this is the direction register to the LED port

#define  SEG7_SELDDR   (DDRG)
#define  SEG7_SELPORT  (PORTG)
#define  SEG7_0        (PG0)
#define  SEG7_1        (PG1)
#define  SEG7_EN       (PG2)
#define  SEG7_GRAD     (PG3)
#define  SEG7_COLON    (PG4)

#define  SEG7_DIGIDDR  (DDRH)
#define  SEG7_DIGIPORT (PORTH)

#define  KEYB_INPORTDDR  (DDRD)
#define  KEYB_INPORT     (PORTD)
#define  KEYB_INMASK     ((1<<PD4)|(1<<PD5)|(1<<PD6))
#define  KEYB_INMASKSHIFT (4)

// The RGB Led's are connects on 


/**
 * @brief    hardware definition for the RGB led
 */
#define ELOB_PORT_RGB     PORTB  // The LEDs are connected to this port
#define ELOB_PORT_DDRRGB  DDRB   //
#define ELOB_RGB_RED      (1<<5) // Pin definition RED
#define ELOB_RGB_GREEN    (1<<6) // Pin definition GREEN
#define ELOB_RGB_BLUE     (1<<7) // Pin definition BLUE

/**
 * @brief    hardware definition for the buzzer
 */
#define BUZZER_ON	   (PORTD |= 0x80)
#define BUZZER_OFF	(PORTD &= 0x7F)


/**
 * @brief    Macro for return code 
 */
#define RET_OK      1   // function successfully executed
#define RET_NOTOK   0   // function execution fails 
#define RET_IDXFAIL 0xff   // function returns a illegal index

#define ON          1   // switch on
#define OFF         0   // switch off

#define BLINK_VAL  120  // blink speed

/**
 * @brief  The baudrate definition of UART1
*/
#define UART1_BAUD_RATE  9600L   


// -----------------------------------------
// Event Definition
// -----------------------------------------

/**
 * @brief    Implemented EVENT definitions
*/

/**
 * @brief    A key was pressed 
*/
#define EVENT_KEY_PRESSED   (0x10)  // a key pressed detected

/**
 * @brief    A key was released
*/
#define EVENT_KEY_RELEASED  (0x11)  // a key released was detected

/**
 * @brief  One second has passed.
*/
#define EVENT_TIM_SECONDE   (0x20)  // each second, this event is generated

/**
 * @brief  An AD conversion is ready.
*/
#define EVENT_ADC_FINISHED  (0x30)  // the ad conversion is finished

/**
 * @brief  The event UART0 indicates that data has been received.
*/
#define EVENT_RXUART0       (0x40)  // UART0 received data

/**
 * @brief  The event UART1 indicates that data has been received.
*/
#define EVENT_RXUART1       (0x41)  // UART1 received data

/**
 * @brief  The buzzer() was started and the time is now reach zero
*/
#define EVENT_BUZZER_OFF    (0x50)  // the buzzer has finished



/**
 * @brief    ADC multiplexer channel for the external 50V voltage measurement
 */
#define ADC_EXT  (1)       // ADC mux input for external measurement

/**
 * @brief    ADC multiplexer channel for the on board NTC
 */
#define ADC_NTC  (2)       // ADC mux input for ntc


/**
 * @brief    ADC multiplexer channel for the on board potentiometer
 */
#define ADC_POTI  (3)       // ADC mux input for the potentiometer


// --------------------------------------------------------
// Macro definition
// --------------------------------------------------------

/**
 *  @brief  The macro definion 
 *  @attention The macros for the RGB led will work only with the elob_init()
 *             initialization. The interaction with the PWM control is not analyzed yet
 */


/**
 * @brief  The macro to initialize the 8-bit LED output 
 */
#define SETUPLED() {ELOB_PORT_DDRLED=0xFF;  ELOB_PORT_LED=0;}

/**
 * @brief  The macro to write a uint8_t to the LED port
 */
#define RGB_SETLED(n)  (ELOB_PORT_LED = n)

/**
 * @brief  The macro to read a uint8_t from the LED port
 */
#define RGB_GETLED()   (ELOB_PORT_LED)



/**
 * @brief  The macro to switch red LED ON
 */
#define RGB_REDON()    (ELOB_PORT_RGB |= ELOB_RGB_RED)

/**
 * @brief  The macro to switch red LED OFF
 */
#define RGB_REDOFF()   (ELOB_PORT_RGB &= ~ELOB_RGB_RED)

/**
 * @brief  The macro to switch green LED ON
 */
#define RGB_GREENON()  (ELOB_PORT_RGB |= ELOB_RGB_GREEN)

/**
 * @brief   The macro to switch green LED OFF
 */
#define RGB_GREENOFF() (ELOB_PORT_RGB &= ~ELOB_RGB_GREEN)

/**
 * @brief  The macro to switch blue LED ON
 */
#define RGB_BLUEON()   (ELOB_PORT_RGB |= ELOB_RGB_BLUE)

/**
 * @brief  The macro to switch blue LED OFF
 */
#define RGB_BLUEOFF()  (ELOB_PORT_RGB &= ~ELOB_RGB_BLUE)



/**
   @brief   The macro to set on the gradient sign in the 7 segment display
*/
#define DISP5SEG_SET_GRAD_ON()   (SEG7_SELPORT |= (1<<SEG7_GRAD))

/**
   @brief   The macro to set off the column sign in the 7 segment display
*/
#define DISP5SEG_SET_GRAD_OFF()  (SEG7_SELPORT &= ~(1<<SEG7_GRAD))

/**
   @brief   The macro to set on the gradient sign in the 7 segment display
*/
#define DISP5SEG_SET_COLON_ON()   (SEG7_SELPORT |= (1<<SEG7_COLON))

/**
   @brief   The macro to set off the column sign in the 7 segment display
*/
#define DISP5SEG_SET_COLON_OFF()  (SEG7_SELPORT &= ~(1<<SEG7_COLON))


// --------------------------------------------------------
// Variable definition
// --------------------------------------------------------





// --------------------------------------------------------
// Function definition
// --------------------------------------------------------



/**
 \ingroup elob_base

   @brief   switch the buzzer on for the given number of 5mS
   @param   uint8_t <tt>Time</tt> in 5mS steps
*/
void  Buzzer(uint8_t Time);

/**
  \ingroup elob_base
   @brief   get a event from the queue 
   @return  uint8_t <tt>0</tt> if no event is available else a <tt>EVENT</tt>
*/
uint8_t deQueue(void);


/**
  \ingroup elob_base
   @brief   put a event into the queue 
   @param   uint8_t <tt>Event</tt> the event to store
   @return  uint8_t <tt>0</tt> if successfully else a <tt>1</tt> (queue full)
*/
uint8_t putQueue(uint8_t Event);



/**
  \ingroup elob_base
   @brief   switches the 7segment display ON or OFF
   @param   uint8_t <tt>On</tt> 
*/
void  disp7Seg_setOnOff(uint8_t On);


/**
   @brief   sets the 7segment display to blinking ON or OFF
   @param   uint8_t <tt>On</tt> 
*/
void disp7Seg_setBlinkOnOff(uint8_t On);

/**
   @details The output of the display has a second memory for the output in blinking OFF mode. With this function the output can be determined.
            In the flashing mode it is then switched between the two memories. 
      
   @brief   tells the 7segment display to write to the OFF or ON memory
   @param   uint8_t <tt>On</tt> 
*/
void disp7Seg_setWriteToOff(uint8_t On);


/**
   @brief   print a number to the 7segment display
   @param   Val   the number to print
   @param   Radix 
   @return  the length of the converted result 
*/
uint8_t disp7Seg_putNumber(int16_t Val, uint8_t Radix);



/**
   @brief   switches ON the decimal dot on the given position
   @param   uint8_t <tt>Pos</tt>  The value range for Pos is from 0 to 3, where 0 means the display on the most right side.
*/
void disp7Seg_setDecimal(int8_t Pos);

/**
   @brief   switches OFF the decimal dot on the given position
   @param   uint8_t <tt>Pos</tt>  The value range for Pos is from 0 to 3, where 0 means the display on the most right side.
*/
void disp7Seg_clearDecimal(int8_t Pos);

/**
   @brief   Clears the 7Seg display content
*/
void disp7Seg_clear(void);

/**
   @brief   puts a character to the 7Seg display
   @param   const uint8_t <tt>C</tt>  The character to print 
*/
void disp7Seg_put(const char C);

/**
   @brief   sets the writing position for the 7Seg display
   @param   uint8_t <tt>Pos</tt>  The value range for Pos is from 0 to 3, where 0 means the display on the most right side.
*/
void disp7Seg_setPos(uint8_t Pos);

/**
   @brief   Initialize all implemented hardware
   @param   
   @return  none
*/
void    elob_init(void);

/**
   @brief   starts a AD conversion
   @param   uint8_t <tt>ADC_Number</tt>  the corresponded multiplexer 
   @return  uint8_t <tt>RET_OK</tt> or <tt>RET_NOTOK</tt> if the ADC is busy
*/
uint8_t  adc_startConversion(uint8_t ADC_Number);

/**
   @brief   read the AD value from the last conversion
   @return  the value from th ADC
*/
uint16_t adc_getValue(void);

/**
   @brief   sets the brightness of the red led
   @param   const int8_t <tt>red</tt> Brightness value for the red led
*/
void  rgb_setRed(const uint8_t red);

/**
   @brief   sets the brightness of the green led
   @param   const int8_t <tt>green</tt> Brightness value for the green led
*/
void    rgb_setGreen(const uint8_t green);

/**
   @brief   sets the brightness of the blue led
   @param   const int8_t <tt>blue</tt> Brightness value for the blue led
*/
void    rgb_set_blue(const uint8_t blue);


/**
   @brief   sets the brightness of all three Led
   @param   const int8_t <tt>red</tt> Brightness value for the red led
   @param   const int8_t <tt>green</tt> Brightness value for the green led
   @param   const int8_t <tt>blue</tt> Brightness value for the blue led

   @attention The RGB leds can be very bright. Do not look directly into the light
*/
void    rgb_set(const uint8_t red,const uint8_t green,const uint8_t blue);

/**
   @brief   gets the key code how was pressed
   @return  uint8_t <tt>keycode</tt> or  <tt>0</tt> if no key is available 
*/
uint8_t key_getPressed(void);

/**
   @brief   gets the key code how was released 
   @return  uint8_t <tt>keycode</tt> or  <tt>0</tt> if no key is available 
*/
uint8_t key_getReleased(void);



/**
   @brief   waits in numbers of 5mS 
   @param   const int8_t  <tt>Time</tt> numbers of 5mS 
*/
void  tim_wait5mS(const int8_t Time);





// ------------------------------------------------------------------
// Tool functions 
// ------------------------------------------------------------------
// 
/**
   @brief   searches a byte in progmem table
   @param   const int8_t <tt>C</tt>  the char to search
   @param   const char* PROGMEM <tt>pTable</tt>  the pointer to the table. The table has to bee terminated by a \0
   @return  uint8_t <tt>index</tt> where the byte was found or 0xff if the byte is not found
*/
uint8_t tool_lookupByte_P(const int8_t C, const char* PROGMEM pTable);


// this is at this time only a dummy function
/**
   @attention  This function is not yet implemented
   @brief   puts a error into the error queue 
   @param   const int8_t <tt>ErrCode</tt>  the char to search
   @return  uint8_t <tt>index</tt> where the byte was found or 0xff if the byte is not found
*/
void    error_put(const uint8_t ErrCode);

/**
   @brief   removes a CrLf at the end of the memory 
   @param   char*  <tt>pBuff</tt>  the memory to process
   @return  uint8_t the new length of the memory
*/
uint8_t remove_atEndCRLF(char* pBuff);

#ifndef fOutFunc
   typedef void   (*fOutFunc)(const char c);
#endif

/**
   @brief   Function for converting a number into a text.
            Examples for the format definition: 
            "-nnnN.nnn"
            + means print sign + or -
            - means print only -

   @param   const int32_t <tt>Num</tt>  The numeric value to be converted to a string.
   @param   const char* <tt>pFormat</tt> The desired format for the output (can be 0)
   @param   fOutFunc <tt>FOutC</tt> The output function to be used
   @return  uint8_t the new length of the written output
*/

int8_t tool_NumFormat(const int32_t Num, const char* pFormat, fOutFunc FOutC);


#endif // ELOB_OUTPUT_H_

