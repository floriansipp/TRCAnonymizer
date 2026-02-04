#ifndef TRCPARAMETERS_H
#define TRCPARAMETERS_H

#include <cstdint>
#include <iostream>

#include "INote.h"

#define MAX_CAN_VIEW 128
#define MAX_NOTE 200
#define MAX_MONT 30

// TRC Header byte offsets and field sizes
namespace TRC {
    // Patient info
    constexpr int SURNAME_OFFSET = 64;
    constexpr int SURNAME_LENGTH = 22;
    constexpr int NAME_OFFSET = 86;
    constexpr int NAME_LENGTH = 20;
    constexpr int BIRTH_MONTH_OFFSET = 106;
    constexpr int BIRTH_DAY_OFFSET = 107;
    constexpr int BIRTH_YEAR_OFFSET = 108;

    // Recording info
    constexpr int RECORD_DAY_OFFSET = 128;
    constexpr int RECORD_MONTH_OFFSET = 129;
    constexpr int RECORD_YEAR_OFFSET = 130;
    constexpr int RECORD_TIME_HOUR_OFFSET = 131;
    constexpr int RECORD_TIME_MIN_OFFSET = 132;
    constexpr int RECORD_TIME_SEC_OFFSET = 133;

    // Area descriptors
    constexpr int NOTES_AREA_DESCRIPTOR_OFFSET = 208;
    constexpr int MONTAGE_AREA_DESCRIPTOR_OFFSET = 288;
    constexpr int MONTAGE_AREA_LENGTH_OFFSET = 300;
    constexpr int AREA_START_OFFSET_DELTA = 8;
    constexpr int AREA_LENGTH_DELTA = 12;

    // Sampling rate
    constexpr int SAMPLING_RATE_OFFSET = 400;
    constexpr int SAMPLING_RATE_SIZE = 2;

    // Year base offset stored as (year - 1900) in file
    constexpr int YEAR_BASE = 1900;

    // Note structure: 44 bytes per note (4 bytes sample + 40 bytes description)
    constexpr int NOTE_SIZE = 44;
    constexpr int NOTE_SAMPLE_SIZE = 4;
    constexpr int NOTE_DESC_OFFSET = 4;
    constexpr int NOTE_DESC_LENGTH = 40;

    // Montage structure: 4096 bytes per montage
    constexpr int MONTAGE_SIZE = 4096;
    constexpr int MONTAGE_DESC_OFFSET = 264;
    constexpr int MONTAGE_DESC_LENGTH = 64;

    // Montage sub-field offsets within a montage block
    constexpr int MONTAGE_COLOUR_OFFSET = 8;
    constexpr int MONTAGE_SELECTION_OFFSET = 136;
    constexpr int MONTAGE_INPUTS_OFFSET = 328;
    constexpr int MONTAGE_HIGHPASS_OFFSET = 840;
    constexpr int MONTAGE_LOWPASS_OFFSET = 1352;
    constexpr int MONTAGE_REFERENCE_OFFSET = 1864;
    constexpr int MONTAGE_FREE_OFFSET = 2376;
    constexpr int MONTAGE_FREE_SIZE = 1720;
}

struct inputOfMontages
{													//Offset
    unsigned short int nonInverting;				//0
    unsigned short int inverting;					//2
};

struct operatorNote : public INote
{
    //Offset - Size - Type
    //Sample - 0 - 4 - unsigned long int
    //Description - 4 - 40 - std::string
    operatorNote() { }
    operatorNote(const INote& note)
    {
        Sample(note.Sample());
        Description(note.Description());
    }
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
