#ifndef BS_FIFO_H
  #define  BS_FIFO_H

/** 
 *  
 *  @file
 *  @defgroup Fifo Fifo BS_Fifo.h
 *  @code #include "BS_Fifo.h" @endcode
 *
 *  @brief  First in first out functions used as a event handler
 *
 *  Detailed description starts here.
 *
 *
 *  @note 
 *  @author Pius Steiner <pius.steiner@gibz.ch> 
 *  @copyright (C) 2018 Pius Steiner, GNU General Public License Version 3
 */


// Zweiter Versuch -> alles in einem Memory
//
// the buffer offsets
#define  FIOFF_TOTLEN  0  //  uint8_t the total length of the circular buffer in bytes
#define  FIOFF_LEN     1  //  uint8_t the stored number
#define  FIOFF_RD      2  //  uint8_t the read offset
#define  FIOFF_WR      3  //  uint8_t the write offset
#define  FIOFF_BUFF    4  //  the Buffer start

/**
   * @brief    initialize the fifo memory
   *
   * @param    Fifo        the fifo memory 
   * @param    sizeofFifo  the defined byte size of the fifo memeory
   * @return   none
   * @see      
   */
uint8_t fifo_init   (uint8_t* Fifo,const uint8_t sizeofFifo);



/**
   * @brief    clear all entrys in the fifo and resets the writing position
   *
   * @param    Fifo        the fifo memory 
   * @return   none
   * @see      
   */
void    fifo_clear(uint8_t* Fifo); // resets rd and wr positions

uint8_t fifo_put(uint8_t*  Fifo, uint8_t C);
uint8_t fifo_get(uint8_t*  Fifo, uint8_t* pC);

// copy ALL data from the fifo to the buffer and clears the buffer
// the function returns the number of copied bytes
// the size of pBuff must 1 byte bigger than the data (\0)
uint8_t fifo_getToBuffer(uint8_t* Fifo, uint8_t* pBuff);

// this function copies the fifo content at the end of the pBuff
uint8_t fifo_appendToBuffer(uint8_t* Fifo, uint8_t* pBuff);


uint8_t fifo_getSize(const uint8_t* Fifo);


// this function compares the last CmpLen characters from the buffer with the pC content
// Returns 0 if no match or not enough data in the fifo
//         1 if matching
uint8_t fifo_compareAtEnd(const uint8_t*  Fifo,const char* pC,uint8_t CmpLen);
uint8_t fifo_compareAtEnd_P(const uint8_t*  Fifo,const char* pC,uint8_t CmpLen);
   
   

#endif // BS_FIFO_H
