/************************************************************************
Title:    Interrupt UART library with receive/transmit circular buffers
Author:   Peter Fleury <pfleury@gmx.ch>  http://tinyurl.com/peterfleury
File:     $Id: uart.h,v 1.13 2015/01/11 13:53:25 peter Exp $
Software: AVR-GCC 4.x, AVR Libc 1.4 or higher
Hardware: any AVR with built-in UART/USART
Usage:    see Doxygen manual

LICENSE:
    Copyright (C) 2015 Peter Fleury, GNU General Public License Version 3

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
************************************************************************/

/** 
 *  @file
 *  @defgroup pfleury_uart UART Library <uart.h>
 *  @code #include <uart.h> @endcode
 * 
 *  @brief Interrupt UART library using the built-in UART with transmit and receive circular buffers. 
 *
 *  This library can be used to transmit and receive data through the built in UART. 
 *
 *  An interrupt is generated when the UART has finished transmitting or
 *  receiving a byte. The interrupt handling routines use circular buffers
 *  for buffering received and transmitted data.
 *
 *  The UART_RX_BUFFER_SIZE and UART_TX_BUFFER_SIZE constants define
 *  the size of the circular buffers in bytes. Note that these constants must be a power of 2.
 *  You may need to adapt these constants to your target and your application by adding 
 *  CDEFS += -DUART_RX_BUFFER_SIZE=nn -DUART_TX_BUFFER_SIZE=nn to your Makefile.
 *
 *  @note Based on Atmel Application Note AVR306
 *  @author Peter Fleury pfleury@gmx.ch  http://tinyurl.com/peterfleury
 *  @copyright (C) 2015 Peter Fleury, GNU General Public License Version 3
 */
 

 #ifndef BS_SER_H_
   #define BS_SER_H_


#include <avr/pgmspace.h>


#define UART_TEST

#define  FERR_UART0_OVER     10   // uart0 overrun error
#define  FERR_UART1_OVER     11   // uart1 overrun error
#define  FERR_UART0_BUFF     12   // uart0 buffer overrun error
#define  FERR_UART1_BUFF     13   // uart1 buffer overrun error
#define  FERR_UART0_FRAME     8   // uart0 framing error
#define  FERR_UART1_FRAME     9   // uart1 framing error



#if (__GNUC__ * 100 + __GNUC_MINOR__) < 405
  #error "This library requires AVR-GCC 4.5 or later, update to newer AVR-GCC compiler !"
#endif

/**@{*/


/*
** constants and macros
*/


/** @brief  UART Baudrate Expression
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000UL for 4Mhz          
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT(baudRate,xtalCpu)  (((xtalCpu) + 8UL * (baudRate)) / (16UL * (baudRate)) -1UL)

/** @brief  UART Baudrate Expression for ATmega double speed mode
 *  @param  xtalCpu  system clock in Mhz, e.g. 4000000UL for 4Mhz           
 *  @param  baudRate baudrate in bps, e.g. 1200, 2400, 9600     
 */
#define UART_BAUD_SELECT_DOUBLE_SPEED(baudRate,xtalCpu) ( ((((xtalCpu) + 4UL * (baudRate)) / (8UL * (baudRate)) -1UL)) | 0x8000)



/** @brief  Size of the circular receive buffer, must be power of 2
 * 
 *  You may need to adapt this constant to your target and your application by adding 
 *  CDEFS += -DUART_RX_BUFFER_SIZE=nn to your Makefile.
 */

 /** @brief  UART0 receive buffer size
 * 
 */
#define UART_RX_BUFFER_SIZE  0

/** @brief  UART0 transmit buffer size
 * 
 */
#define UART_TX_BUFFER_SIZE   0

/** @brief  UART1 receive buffer size
 * 
 */
#define UART1_RX_BUFFER_SIZE 128

/** @brief  UART1 transmit buffer size
 * 
 */
#define UART1_TX_BUFFER_SIZE  64




/* test if the size of the circular buffers fits into SRAM */
#if ( (UART_RX_BUFFER_SIZE+UART_TX_BUFFER_SIZE) >= (RAMEND-0x60 ) )
  #error "size of UART_RX_BUFFER_SIZE + UART_TX_BUFFER_SIZE larger than size of SRAM"
#endif

/* 
** high byte error return code of uart_getc()
*/
#define UART_FRAME_ERROR      0x1000              /**< @brief Framing Error by UART        */
#define UART_OVERRUN_ERROR    0x0800              /**< @brief Overrun condition by UART    */
#define UART_PARITY_ERROR     0x0400              /**< @brief Parity Error by UART         */ 
#define UART_BUFFER_OVERFLOW  0x0200              /**< @brief receive ring buffer overflow */
#define UART_NO_DATA          0x0100              /**< @brief no receive data available    */

#define UART_FLAG_1ECHOON     1
#define UART1_RXWAIT          (5)  // waits for 5*20ms before fire a event RX

extern   uint8_t uart0_getToBuff(char* pBuff);
extern   uint8_t uart1_getToBuff(char* pBuff);

//extern   void    uart_setEcho1(const uint8_t Flag);
   /**
      @brief   Initialize UART and set baudrate 
      @param   baudrate Specify baudrate using macro UART_BAUD_SELECT()
      @return  none
   */
extern void uart_init(unsigned int baudrate);
   
    /**
    *  @brief   Get received byte from ringbuffer
    *
    * Returns in the lower byte the received character and in the 
    * higher byte the last receive error.
    * UART_NO_DATA is returned when no data is available.
    *
    *  @return  lower byte:  received byte from ringbuffer
    *  @return  higher byte: last receive status
    *           - \b 0 successfully received data from UART
    *           - \b UART_NO_DATA           
    *             <br>no receive data available
    *           - \b UART_BUFFER_OVERFLOW   
    *             <br>Receive ringbuffer overflow.
    *             We are not reading the receive buffer fast enough, 
    *             one or more received character have been dropped 
    *           - \b UART_OVERRUN_ERROR     
    *             <br>Overrun condition by UART.
    *             A character already present in the UART UDR register was 
    *             not read by the interrupt handler before the next character arrived,
    *             one or more received characters have been dropped.
    *           - \b UART_FRAME_ERROR       
    *             <br>Framing Error by UART
    */
extern    unsigned int uart_getc(void);


   /**
    *  @brief   Put byte to ringbuffer for transmitting via UART
    *  @param   data byte to be transmitted
    *  @return  none
    */
extern   void uart_putc( char data);

   /**
    *  @brief   Put string to ringbuffer for transmitting via UART
    *
    *  The string is buffered by the uart library in a circular buffer
    *  and one character at a time is transmitted to the UART using interrupts.
    *  Blocks if it can not write the whole string into the circular buffer.
    * 
    *  @param   s string to be transmitted
    *  @return  none
    */
extern   void uart_puts(const char *s );

   /**
    * @brief    Put string from program memory to ringbuffer for transmitting via UART.
    *
    * The string is buffered by the uart library in a circular buffer
    * and one character at a time is transmitted to the UART using interrupts.
    * Blocks if it can not write the whole string into the circular buffer.
    *
    * @param    s program memory string to be transmitted
    * @return   none
    * @see      uart_puts_P
    */
extern   void uart_puts_p(const char *s );
/** @brief  Initialize USART1 (only available on selected ATmegas) @see uart_init */
extern    void uart1_init(unsigned int baudrate);
/** @brief  Get received byte of USART1 from ringbuffer. (only available on selected ATmega) @see uart_getc */
extern    unsigned int uart1_getc(void);
/** @brief  Put byte to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_putc */
extern    void uart1_putc(const char data);
/** @brief  Put string to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts */
extern    void uart1_puts(const char *s );
/** @brief  Put string from program memory to ringbuffer for transmitting via USART1 (only available on selected ATmega) @see uart_puts_p */
extern    void uart1_puts_p(const char *s );
/** @brief  Macro to automatically put a string constant into program memory */
#define uart1_puts_P(__s)       uart1_puts_p(PSTR(__s))



/**
 * @brief    Macro to automatically put a string constant into program memory
 */
#define uart_puts_P(__s)    uart_puts_p(PSTR(__s))


#endif  //    define BS_SER_H_


