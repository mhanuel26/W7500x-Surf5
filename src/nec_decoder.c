/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "sst.h"           /* SST framework */
#include "bsp.h"           /* Board Support Package interface */
#include "led_matrix.h"        /* application shared interface */
#include "nec_decoder.h"

volatile uint32_t packet = 0;	    // Last received packet buffer (4 bytes)
volatile uint32_t buffer = 0;		// Buffer of currently received packet (4 bytes)
volatile uint32_t tmr = 0;			// Time of previous falling edge (us)
volatile int8_t counter = 32;		// Bit counter in received packet
volatile int8_t repeats = 0;        // repeat counter
volatile bool start = false;        // start flag
volatile bool decoded = false;		// Data readiness flag (packet received and decoded)
volatile bool repeat = false;		// Repeat flag (remote sent the same thing as last time)
volatile bool parity = false;		// Parity flag


void nec_decoder_tick(void){
    uint32_t time = micros() - tmr;                // we get the time of the last pulse
    tmr += time;                                   // reset the timer (== (tmr = micros() ))
    if (time > 150000U) repeats = -1;               // replay timeout
    if (start && time < _NEC_HIGH_MAX) {           //  reading: date
        uint8_t mode = 2;
        if (time > _NEC_LOW_MIN && time < _NEC_LOW_MAX) mode = 0;           // LOW after all
        else if (time > _NEC_HIGH_MIN && time < _NEC_HIGH_MAX) mode = 1;    // HIGH after all
        if (mode != 2) {                            // HIGH or LOW
            buffer = buffer << 1 | mode;          // write to the buffer
            if (mode) parity = !parity;           // parity
            if (++counter == 32) {                 // if accepted 32 bits
                if (parity) return;                // parity does not match - the packet is broken
                if (((buffer >> 8) & buffer) & 0xFF00FF) return;  // package is broken
                packet = buffer;                  // move the packet from the buffer to the date
                decoded = true;                    // flag for successful packet reception
                repeats = 0;                       // reset the retry counter
                start = false;                     // reading finished
                return;
            }
        }
    }
    if (repeats != -1 && time > _NEC_REPEAT_MIN && time < _NEC_REPEAT_MAX) {   // reading: repeat
        if (repeats > _NEC_SKIP_REPEAT) repeat = true;
        else repeats++;
    } else if (time > _NEC_START_MIN && time < _NEC_START_MAX) {    // read: start of packet
        buffer = 0;
        parity = 0;
        counter = 0;                           // reset everything
        start = true;
    }
}


bool available(void) {
    if (decoded || repeat) {
        decoded = repeat = false;
        return true;
    } return false;
}

bool isDecoded(void) {
    if (decoded) {
        decoded = false;
        return true;
    } return false;
}

bool isRepeated(void) {
    if (repeat) {
        repeat = false;
        return true;
    } return false;
}

uint32_t readPacket(void) {
    return packet;
}

uint8_t readAddress(void) {
    return ((uint32_t)packet >> 24);
}

uint8_t readCommand(void) {
    return ((uint32_t)packet >> 8 & 0xFF);
}

uint8_t readInvCommand(void) {
    return ((uint32_t)packet & 0xFF);
}

uint8_t readInvAddress(void) {
    return ((uint32_t)packet >> 16 & 0xFF);
}

bool addressIsValid(void) {
    return true;
}

bool commandIsValid(void) {
    return true;
}

bool packetIsValid() {
    return true;
}
