
/*
DynaSinTable.h
Dynamic Sin wave Look up Table
	DynaSinTable_lay() :
		connect DynaSinTable structure, Table initial signal vector S0_p, Signal Table matrix S_p, update parameter vector TwoCos_p and result signal vector
		Search for and initialize Signal Table matrix S_p and Table section vector N[]
	DynaSinTable_step() :
		recursive update Dynamic Signal talbe S_p[] and Table section vector N[]
		interpolation calculate signal vector s_p[] with current S_p[] and N[]
*/

#ifndef DynaSinTable_H_INCLUDED
#define DynaSinTable_H_INCLUDED
#include <stdint.h>
#include <stdio.h>
#include <math.h>
typedef struct DynaSinTable {
	uint16_t n;						//F16.0 count at this cycle
	uint16_t last_val;
	uint16_t N[3];					//F16.0 dynamic sin table current count vector
	uint16_t FullScale;				//Counts per Cycle 
	uint16_t CountPerPeri;			//Count per period of look up table
	uint8_t Channel;				//channel of sin ware to look up 
	uint8_t DiviShift;				//Interpolation division shift 
	uint32_t* FullCountIn_p;//F32.0 address of the input count variable
	int32_t* S0_p;			//F2.30 dynamic sin table initial sin value matrix 
	int32_t* TwoCos_p;		//F3.29 dynamic iteration update parameter vector
	int32_t* S_p;			//F2.30 dynamic sin table current sin value matrix
	int32_t* s_p;			//F2.30 sin values of all channels at the input count 
}DynaSinTableStr_t;

/*count per period : Dynamic look up table interval count*/
#define R_EXPON 31
#define PI 3.14159265358979323846

void DynaSinTable_lay(DynaSinTableStr_t* Str_p, int32_t* S0_p, int32_t* S_p, int32_t* TwoCos_p, int32_t* s_p);
uint8_t DynaSinTable_step(void* void_p);


#endif //DynaSinTable_H_INCLUDED