/* frameHandler.h
 *
 * Arduino library to read from Victron devices using VE.Direct protocol.
 * Derived from Victron framehandler reference implementation.
 * 
 * 2020.05.05 - 0.2 - initial release
 * 
 */

#ifndef FRAMEHANDLER_H_
#define FRAMEHANDLER_H_

const byte frameLen = 18;//by issue use new length 22                       // VE.Direct Protocol: max frame size is 18
//see https://community.victronenergy.com/questions/48935/vedirect-library-for-arduino.html
const byte nameLen = 9;                         // VE.Direct Protocol: max name size is 9 including /0
const byte valueLen = 33;                       // VE.Direct Protocol: max value size is 33 including /0
const byte buffLen = 40;                        // Maximum number of lines possible from the device. Current protocol shows this to be the BMV700 at 33 lines.


class VeDirectFrameHandler {

public:
    VeDirectFrameHandler();
    void rxData(uint8_t inbyte);                // byte of serial data to be passed by the application

    char veName[buffLen][nameLen] = { };        // public buffer for received names
    char veValue[buffLen][valueLen] = { };      // public buffer for received values

    int frameIndex;                             // which line of the frame are we on
    int frameCount;                             // total full frame count
    int veEnd;                                  // current size (end) of the public buffer

private:
    //bool mStop;                               // not sure what Victron uses this for, not using

    enum States {                               // state machine
        IDLE,
        RECORD_BEGIN,
        RECORD_NAME,
        RECORD_VALUE,
        CHECKSUM,
        RECORD_HEX
    };

    int mState;                                 // current state

    uint8_t	mChecksum;                          // checksum value

    char * mTextPointer;                        // pointer to the private buffer we're writing to, name or value

    char mName[9];                              // buffer for the field name
    char mValue[33];                            // buffer for the field value
    char tempName[frameLen][nameLen];           // private buffer for received names
    char tempValue[frameLen][valueLen];         // private buffer for received values

    void textRxEvent(char *, char *);
    void frameEndEvent(bool);
    void logE(char *, char *);
    bool hexRxEvent(uint8_t);
};

#endif // FRAMEHANDLER_H_
