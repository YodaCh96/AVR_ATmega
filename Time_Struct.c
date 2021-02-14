#include <avr/io.h>
#include <stdio.h>   // for NULL definition
#include <stdlib.h>  // for qsort

#include "elob_base.h"

char RxBuff[100];

uint8_t key;


// the structure definition for an appointment
typedef struct{
	uint8_t h;
	uint8_t min;
	uint8_t s;
	char Txt[16];
}structZeit;

// a list of appointments
structZeit Termine[] = {
	{20, 15, 00,  "Essen"},
	{8,  00, 00,  "KurzPause"},
	{12, 10, 00, "MittagsPause"},
	{7,  50, 00,  "Lektion1"}
 };

// determining the number of entries
#define TERMIN_CNT ( sizeof(Termine) / sizeof(structZeit) )

// determining the number of entries
structZeit *pTerminArr[TERMIN_CNT] = {
	&Termine[0],
	&Termine[1],
	&Termine[2],
	&Termine[3]
};

// the compare function called from the qsort function
int compareTermin(const void *pL, const void *pR)
{
	//++CompareCnt;
	
	int Diff = 0;
	
	const StructZeit = * pLeft = *(const StructZeit **)pL;
	const StructZeit = * pRight = *(const StructZeit **)pR;
	
	if(Diff = (pLeft->h - pRight->h) !=0)
	return Diff;
	else if(Diff = (pLeft->min - pRight->min) !=0)
	return Diff;
	else if(Diff = (pLeft->s - pRight->s) !=0)
	return Diff;
	
	return 0;
}//compareTermin

structZeit Time = {5,20,30};

 int main(void)
{
	uint8_t Err, Event;
	//INIT_TESTPIN();
	
	// no call in simulation mode
	// elob_init();                  // initialize the hardware of ELOB
	
	qsort(pTerminArr, TERMIN_CNT, sizeof(structZeit *) , compareTermin);
	
	const structZeit *pZeit;
	
	for(uint8_t i=0; i< TERMIN_CNT; i++) {
		pZeit = pTerminArr[i];
		uart1_puts(pZeit->Txt);
	    }
}//main