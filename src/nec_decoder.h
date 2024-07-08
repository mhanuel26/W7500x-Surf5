/*
    NEC DECODER implementation in C.
    Manuel Iglesias
    MIT License

    Original code taked from 
    https://github.com/GyverLibs/NecDecoder

*/

#ifndef NEC_DECODER_H_
#define NEC_DECODER_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define _NEC_TOLERANCE 150      // допуск high/low, мкс
#define _NEC_TOLERANCE2 1500    // допуск start/repeat, мкс
#define _NEC_SKIP_REPEAT 2      // пропуск первых повторов, шт  - skipping first repetitions

// Тайминги NEC, мкс
#define _NEC_HIGH_BIT 	2250    // 2.25ms 
#define _NEC_LOW_BIT	1150    // 1.150ms
#define _NEC_START_BIT  14400   // 14.4ms   (9ms + 4.5ms + pulse burst of 562.5us  = 14.0625 ms)
#define _NEC_REPEAT		12300

// =========================================================================
#define _NEC_HIGH_MIN (_NEC_HIGH_BIT - _NEC_TOLERANCE)
#define _NEC_HIGH_MAX (_NEC_HIGH_BIT + _NEC_TOLERANCE)
#define _NEC_LOW_MIN (_NEC_LOW_BIT - _NEC_TOLERANCE)
#define _NEC_LOW_MAX (_NEC_LOW_BIT + _NEC_TOLERANCE)

#define _NEC_START_MIN (_NEC_START_BIT - _NEC_TOLERANCE2)   // 14.4ms - 1.5ms = 12.9ms
#define _NEC_START_MAX (_NEC_START_BIT + _NEC_TOLERANCE2)   // 14.4ms + 1.5ms = 15.9ms
#define _NEC_REPEAT_MIN (_NEC_REPEAT - _NEC_TOLERANCE2)
#define _NEC_REPEAT_MAX (_NEC_REPEAT + _NEC_TOLERANCE2)


void nec_decoder_tick(void);
bool available(void);
bool isDecoded(void);
bool isRepeated(void);
uint32_t readPacket(void);
uint8_t readAddress(void);
uint8_t readCommand(void);
uint8_t readInvCommand(void);
uint8_t readInvAddress(void);
bool addressIsValid(void);
bool commandIsValid(void);
bool packetIsValid(void);

#endif
