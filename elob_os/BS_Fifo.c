
#include <avr/io.h>           // include I/O definitions (port names, pin names, etc)
#include <avr/pgmspace.h>     // include interrupt support
#include <string.h>           // include interrupt support
#include "BS_Fifo.h"


// =======================================================
// Implementation circular buffer FIFO
// =======================================================
// the buffer offsets


// the function returns the resulting fifo size
//
uint8_t fifo_init(uint8_t* Fifo,const uint8_t sizeofFifo)
{
   Fifo[FIOFF_TOTLEN] = sizeofFifo-FIOFF_BUFF;
   Fifo[FIOFF_RD]     = Fifo[FIOFF_WR] = Fifo[FIOFF_LEN] = 0;
   return Fifo[FIOFF_TOTLEN];
}


void fifo_clear(uint8_t* Fifo)
{
   Fifo[FIOFF_RD] = Fifo[FIOFF_WR] = Fifo[FIOFF_LEN] = 0;
}


uint8_t fifo_put(uint8_t* Fifo, uint8_t C)
{
   if(Fifo[FIOFF_LEN] == Fifo[FIOFF_TOTLEN])
      return 1; // buffer is full

   Fifo[FIOFF_BUFF + Fifo[FIOFF_WR]] = C;
   ++Fifo[FIOFF_WR];

   if(Fifo[FIOFF_WR] >= Fifo[FIOFF_TOTLEN])
      Fifo[FIOFF_WR] = 0;

   ++Fifo[FIOFF_LEN];

   return 0;
}

uint8_t fifo_get(uint8_t* Fifo, uint8_t* pC)
{
   if(Fifo[FIOFF_LEN] == 0)
       return 1; // no data available

   *pC = Fifo[FIOFF_BUFF + Fifo[FIOFF_RD]];

   ++Fifo[FIOFF_RD];
   if(Fifo[FIOFF_RD] >= Fifo[FIOFF_TOTLEN])
      Fifo[FIOFF_RD] = 0;

   --Fifo[FIOFF_LEN];

   return 0;
}

// cpy ALL data from the fifo to the buffer and clears the buffer
// the function returns the number of copied bytes
// the size of pBuff must 1 byte bigger than the data (\0)
uint8_t fifo_getToBuffer(uint8_t* Fifo, uint8_t* pBuff)
{
   uint8_t Ret, Cnt = Fifo[FIOFF_LEN];
   Ret = Cnt;
   if(Cnt != 0) {
	  while(Cnt--) {
         fifo_get(Fifo, pBuff++);
	  }
   }
   *pBuff = 0;
   return Ret;
}

uint8_t fifo_appendToBuffer(uint8_t* Fifo, uint8_t* pBuff)
{
   uint8_t Len = strlen((char*)pBuff);
   return  fifo_getToBuffer(Fifo, pBuff+Len);
}



uint8_t fifo_getSize(const uint8_t* Fifo)
{
   return Fifo[FIOFF_LEN];
}


uint8_t fifo_compareAtEnd(const uint8_t*  Fifo,const char* pC,uint8_t CmpLen)
{
   uint8_t Rd;

   if(Fifo[FIOFF_LEN] >= CmpLen) {
      Rd = Fifo[FIOFF_WR];   // last character 
      do {
         Rd = (Rd == 0) ? ((Fifo[FIOFF_TOTLEN])-1) : Rd-1;

         if(pC[CmpLen-1] != Fifo[Rd+FIOFF_BUFF])
		    break;

		 --CmpLen;
      }while(CmpLen);
      return !CmpLen;
   }
   return 0;
}


uint8_t fifo_compareAtEnd_P(const uint8_t*  Fifo,const char* pC,uint8_t CmpLen)
{
   uint8_t Rd;

   if(Fifo[FIOFF_LEN] >= CmpLen) {
      Rd = Fifo[FIOFF_WR];   // last character 
      do {
         Rd = (Rd == 0) ? ((Fifo[FIOFF_TOTLEN])-1) : Rd-1;
         if(pgm_read_byte((pC+CmpLen-1)) != Fifo[Rd+FIOFF_BUFF])
		    break;
		 --CmpLen;
      }while(CmpLen);
      return !CmpLen;
   }
   return 0;
}



