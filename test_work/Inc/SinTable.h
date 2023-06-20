/* SinTable.h

    Usage :
      0. Modify the configulable macro according to the situation
      1. include this file
      2. call SinTable_LAY()   to layout and initialize the parameters
        SinTableStr_t* Str_p,  str with parameters
      3. call SinTable()
        SinTableStr_t* Str_p,
        uint16_t Deg,         input angle in degree
        return int16         result sinwave
*/

#ifndef SinTable_H_INCLUDED
#define SinTable_H_INCLUDED
#include <stdint.h>
#include <math.h>

typedef struct SinTable {
    int16_t sin16u; //sin(8u) in F2.14
    int16_t sin32u; //sin(8u) in F2.14
    int16_t sin48u; //sin(56u) in F2.14
}SinTableStr_t;

/*Declear lay out function  */
void SinTable_ini(SinTableStr_t* Str_p);
int16_t SinTable(SinTableStr_t* Str_p, uint16_t Deg);

extern SinTableStr_t SinTable_str;

#define SinTable_LAY(){\
    SinTable_ini(&SinTable_str);\
}

#endif //SinTable_H_INCLUDED