#pragma once

//#include <stdint.h>
//#include <functional>
#ifndef FRAMEHANDLER_H_
#define FRAMEHANDLER_H_

class VeDirectFrameHandler
{
public:
	static const size_t buffLen = 40;           // Maximum number of lines possible from the device.
	static const size_t frameLen = buffLen;     // Temp frame buffers must not be smaller than public buffers.
	static const size_t nameLen = 9;            // VE.Direct Protocol: max name size is 9 including /0
	static const size_t valueLen = 33;          // VE.Direct Protocol: max value size is 33 including /0
	static const size_t rawFrameLen = 512;      // Raw frame capture for CRC diagnostics.
	static const uint32_t crcDetailLogIntervalMs = 5000;




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
	uint8_t rawFrame[rawFrameLen] = { };        // raw bytes used for CRC diagnostics
	size_t rawFrameIndex = 0;                   // captured raw byte count
	size_t rawFrameBytes = 0;                   // total raw byte count
	bool rawFrameStarted = false;               // true after first captured byte
	bool rawFrameOverflow = false;              // true if raw capture was truncated
	uint32_t lastCrcDetailLogMs = 0;            // detailed CRC log throttle
	uint16_t suppressedCrcLogCount = 0;         // CRC logs suppressed by throttle

	void textRxEvent(char *, char *);
	void frameEndEvent(bool, uint8_t, uint8_t, uint8_t);
	bool hexRxEvent(uint8_t);
	void resetRawFrame();
	void captureRawFrameByte(uint8_t);
	void logChecksumFailure(uint8_t, uint8_t, uint8_t);
	void logEscapedRawFrame();
};
#endif // FRAMEHANDLER_H_
