#ifndef TRCPARAMETERS_H
#define TRCPARAMETERS_H

#include <iostream>

#define MAX_CAN_VIEW 128
#define MAX_MONT 30

struct inputOfMontages
{													//Offset
    unsigned short int nonInverting;				//0
    unsigned short int inverting;					//2
};

struct montagesOfTrace
{													//Offset
    unsigned short int lines;						//0
    unsigned short int sectors;						//2
    unsigned short int baseTime;					//4
    unsigned short int notch;						//6
    unsigned char colour[MAX_CAN_VIEW];				//8
    unsigned char selection[MAX_CAN_VIEW];			//136
    char description[64];							//264
    inputOfMontages inputs[MAX_CAN_VIEW];			//328
    uint32_t highPassFilter[MAX_CAN_VIEW];          //840
    uint32_t lowPassFilter[MAX_CAN_VIEW];           //1352
    uint32_t reference[MAX_CAN_VIEW];               //1864
    unsigned char free[1720];						//2376
};

#endif // TRCPARAMETERS_H
