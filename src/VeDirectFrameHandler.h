#pragma once

//#include <stdint.h>
//#include <functional>
#ifndef FRAMEHANDLER_H_
#define FRAMEHANDLER_H_

class VeDirectFrameHandler
{
public:
	static const size_t frameLen = 22;          // VE.Direct Protocol: max frame size is 22
	static const size_t nameLen = 9;            // VE.Direct Protocol: max name size is 9 including /0
	static const size_t valueLen = 33;          // VE.Direct Protocol: max value size is 33 including /0
	static const size_t buffLen = 40;           // Maximum number of lines possible from the device. Current protocol shows this to be the




    //VeDirectFrameHandler();
    //~VeDirectFrameHandler();
    //void setErrorHandler(logFunction f) { logEF = f; } // error handler
    void rxData(uint8_t inbyte);                       // byte of serial data to be passed by the application
    //void addHexCallback(hexFrameCallback, void *);          // add function called back when hex frame is ready (sync or async)
    void callback(std::function<void()> func);         // callback function
    std::function<void()> requestCallback;
/*
	constexpr VeDirectFrameHandler() = default;
	void rxData(uint8_t inbyte);                // byte of serial data to be passed by the application
	std::function<void(bool valid, char const * name)> recordNameCallback = [](bool, char const *){ };
	std::function<void(bool valid, char const * value)> recordValueCallback = [](bool, char const *){ };
	std::function<void(uint8_t checksum)> checksumCallback = [](uint8_t){ };
	std::function<void()> hexFrameCallback = [](){ };
	std::function<void(bool valid)> frameEndEventCallback = [](bool){ };

    void callback(std::function<void()> func);         // callback function
    std::function<void()> requestCallback;
    */

	char veName[buffLen][nameLen] = { };        // public buffer for received names
	char veValue[buffLen][valueLen] = { };      // public buffer for received values

	size_t veEnd = 0;                           // current size (end) of the public buffer
    int veErrorTol = 10;                            // error counter for crc, if higher than error tollerance 
    int veErrorCount = 0;   // crc error counter
    bool veError = true;                // error flag for crc

private:
	enum States {                               // state machine
		IDLE,
		RECORD_BEGIN,
		RECORD_NAME,
		RECORD_VALUE,
		CHECKSUM,
		RECORD_HEX
	};

	int mState = IDLE;                          // current state
	uint8_t mChecksum = 0;                      // checksum value
	int frameIndex = 0;                         // which line of the frame are we on

	char * mTextPointer = nullptr;              // pointer to the private buffer we're writing to, name or value

	char mName[nameLen] = { };                  // buffer for the field name
	char mValue[valueLen] = { };                // buffer for the field value
	char tempName[frameLen][nameLen] = { };     // private buffer for received names
	char tempValue[frameLen][valueLen] = { };   // private buffer for received values

	void textRxEvent(char *, char *);
	void frameEndEvent(bool);
	bool hexRxEvent(uint8_t);
};
#endif // FRAMEHANDLER_H_
