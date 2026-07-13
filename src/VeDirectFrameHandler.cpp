#include <Arduino.h>
#include "VeDirectFrameHandler.h"

extern void writeLog(const char* format, ...);
// The name of the record that contains the checksum.
static constexpr char checksumTagName[] = "CHECKSUM";

/*
 *  rxData
 *  This function is called by the application which passes a byte of serial data
 *  It is unchanged from Victron's example code
 */
void VeDirectFrameHandler::rxData(uint8_t inbyte)
{
	//Serial.write(inbyte);
	const uint8_t rawByte = inbyte;
	if ((rawByte == ':') && (mState != CHECKSUM))
	{
		mState = RECORD_HEX;
		resetRawFrame();
	}
	if (mState != RECORD_HEX)
	{
		captureRawFrameByte(rawByte);
		mChecksum += rawByte;
	}
	inbyte = toupper(rawByte);

	switch (mState)
	{
	case IDLE:
		/* wait for \n of the start of an record */
		switch (inbyte)
		{
		case '\n':
			mState = RECORD_BEGIN;
			break;
		case '\r': /* Skip */
		default:
			break;
		}
		break;
	case RECORD_BEGIN:
		mTextPointer = mName;
		*mTextPointer++ = inbyte;
		mState = RECORD_NAME;
		break;
	case RECORD_NAME:
		// The record name is being received, terminated by a \t
		switch (inbyte)
		{
		case '\t':
			// the Checksum record indicates a EOR
			if (mTextPointer < (mName + sizeof(mName)))
			{
				*mTextPointer = 0; /* Zero terminate */
				//recordNameCallback(true, mName);
				if (strcmp(mName, checksumTagName) == 0)
				{
					mState = CHECKSUM;
					break;
				}
			} else {
				//recordNameCallback(false, nullptr);
			}
			mTextPointer = mValue; /* Reset value pointer */
			mState = RECORD_VALUE;
			break;


		default:
			// add byte to name, but do no overflow
			if (mTextPointer < (mName + sizeof(mName)))
				*mTextPointer++ = inbyte;
			break;
		}
		break;
	case RECORD_VALUE:
		// The record value is being received.  The \r indicates a new record.
		switch (inbyte)
		{
		case '\n':
			// forward record, only if it could be stored completely
			if (mTextPointer < (mValue + sizeof(mValue)))
			{
				*mTextPointer = 0; // make zero ended
				//recordValueCallback(true, mValue);
				textRxEvent(mName, mValue);
			} else {
				//recordValueCallback(false, nullptr);
			}
			mState = RECORD_BEGIN;
			break;
		case '\r': /* Skip */
			break;
		default:
			// add byte to value, but do no overflow
			if (mTextPointer < (mValue + sizeof(mValue)))
				*mTextPointer++ = inbyte;
			break;
		}
		break;
	case CHECKSUM:
	{
		//checksumCallback(mChecksum);
		bool valid = mChecksum == 0;
		const uint8_t receivedChecksum = rawByte;
		const uint8_t checksumTotal = mChecksum;
		const uint8_t checksumWithoutReceived = checksumTotal - receivedChecksum;
		const uint8_t calculatedChecksum = 0 - checksumWithoutReceived;
		mChecksum = 0;
		mState = IDLE;
		frameEndEvent(valid, receivedChecksum, calculatedChecksum, checksumTotal);
		break;
	}
	case RECORD_HEX:
		if (hexRxEvent(inbyte))
		{
			//hexFrameCallback();
			mChecksum = 0;
			mState = IDLE;
			resetRawFrame();
		}
		break;
	}
}

void VeDirectFrameHandler::resetRawFrame()
{
	rawFrameIndex = 0;
	rawFrameBytes = 0;
	rawFrameStarted = false;
	rawFrameOverflow = false;
}

void VeDirectFrameHandler::captureRawFrameByte(uint8_t inbyte)
{
	if (!rawFrameStarted)
	{
		rawFrameStarted = true;
	}

	rawFrameBytes++;
	if (rawFrameIndex < rawFrameLen)
	{
		rawFrame[rawFrameIndex++] = inbyte;
	}
	else
	{
		rawFrameOverflow = true;
	}
}

/*
 * textRxEvent
 * This function is called every time a new name/value is successfully parsed.  It writes the values to the temporary buffer.
 */
void VeDirectFrameHandler::textRxEvent(char *mName, char *mValue)
{
	if (frameIndex >= (int)frameLen) {
		// Drop excess lines to avoid temp buffer overflow.
		return;
	}

	// Copy with bounds and force NUL termination.
	strncpy(tempName[frameIndex], mName, nameLen - 1);
	tempName[frameIndex][nameLen - 1] = '\0';
	strncpy(tempValue[frameIndex], mValue, valueLen - 1);
	tempValue[frameIndex][valueLen - 1] = '\0';
	frameIndex++;
}

/*
 *  frameEndEvent
 *  This function is called at the end of the received frame.  If the checksum is valid, the temp buffer is read line by line.
 *  If the name exists in the public buffer, the new value is copied to the public buffer.  If not, a new name/value entry
 *  is created in the public buffer.
 */
void VeDirectFrameHandler::frameEndEvent(bool valid, uint8_t receivedChecksum, uint8_t calculatedChecksum, uint8_t checksumTotal)
{
	if (valid)
	{
		for (int i = 0; i < frameIndex && i < (int)frameLen; i++)
		{
			// read each name already in the temp buffer
			bool nameExists = false;
			for (size_t j = 0; j < veEnd; j++)
			{
				// compare to existing names in the public buffer
				if (strcmp(tempName[i], veName[j]) == 0)
				{
					strcpy(veValue[j], tempValue[i]); // overwrite tempValue in the public buffer
					nameExists = true;
					break;
				}
			}
			if (!nameExists)
			{
				strcpy(veName[veEnd], tempName[i]);   // write new Name to public buffer
				strcpy(veValue[veEnd], tempValue[i]); // write new Value to public buffer
				veEnd++;                              // increment end of public buffer
				if (veEnd >= buffLen)
				{ // stop any buffer overrun
					veEnd = buffLen - 1;
				}
			}
		}
		veErrorCount = 0;
		requestCallback(); // call the callback to do other things with the new data
	} else {
		veErrorCount++;
		logChecksumFailure(receivedChecksum, calculatedChecksum, checksumTotal);
	}
	frameIndex = 0; // reset frame
	veError = veErrorCount < veErrorTol ? false : true; //set the crc error flag
	resetRawFrame();
}

bool VeDirectFrameHandler::hexRxEvent(uint8_t inbyte)
{
	return inbyte == '\n';
}

void VeDirectFrameHandler::callback(std::function<void()> func) // callback function when finnish request
{
	requestCallback = func;
}

void VeDirectFrameHandler::logChecksumFailure(uint8_t receivedChecksum, uint8_t calculatedChecksum, uint8_t checksumTotal)
{
	const uint32_t now = millis();
	if (lastCrcDetailLogMs != 0 && now - lastCrcDetailLogMs < crcDetailLogIntervalMs)
	{
		if (suppressedCrcLogCount < UINT16_MAX)
			suppressedCrcLogCount++;
		return;
	}

	const uint16_t suppressed = suppressedCrcLogCount;
	suppressedCrcLogCount = 0;
	lastCrcDetailLogMs = now;

	writeLog("[VE] CRC wrong bytes=%u captured=%u truncated=%s suppressed=%u",
			 (unsigned)rawFrameBytes,
			 (unsigned)rawFrameIndex,
			 rawFrameOverflow ? "yes" : "no",
			 (unsigned)suppressed);
	writeLog("[VE] CRC received=0x%02X calculated=0x%02X sum=0x%02X",
			 receivedChecksum,
			 calculatedChecksum,
			 checksumTotal);
	logEscapedRawFrame();
}

void VeDirectFrameHandler::logEscapedRawFrame()
{
	writeLog("[VE] frame escaped begin bytes=%u", (unsigned)rawFrameIndex);

	char line[160];
	size_t pos = 0;
	size_t chunkStart = 0;
	unsigned chunk = 0;
	for (size_t i = 0; i < rawFrameIndex; i++)
	{
		char token[5];
		size_t tokenLen = 0;
		const uint8_t b = rawFrame[i];

		switch (b)
		{
		case '\r':
			token[0] = '\\'; token[1] = 'r'; tokenLen = 2;
			break;
		case '\n':
			token[0] = '\\'; token[1] = 'n'; tokenLen = 2;
			break;
		case '\t':
			token[0] = '\\'; token[1] = 't'; tokenLen = 2;
			break;
		case '\\':
			token[0] = '\\'; token[1] = '\\'; tokenLen = 2;
			break;
		default:
			if (b >= 0x20 && b <= 0x7E)
			{
				token[0] = (char)b;
				tokenLen = 1;
			}
			else
			{
				snprintf(token, sizeof(token), "\\x%02X", b);
				tokenLen = 4;
			}
			break;
		}

		if (pos + tokenLen >= sizeof(line))
		{
			line[pos] = '\0';
			writeLog("[VE] frame escaped chunk=%u offset=%u %s",
					 chunk++,
					 (unsigned)chunkStart,
					 line);
			pos = 0;
			chunkStart = i;
		}

		memcpy(line + pos, token, tokenLen);
		pos += tokenLen;
	}

	if (pos > 0)
	{
		line[pos] = '\0';
		writeLog("[VE] frame escaped chunk=%u offset=%u %s",
				 chunk++,
				 (unsigned)chunkStart,
				 line);
	}

	writeLog("[VE] frame escaped end chunks=%u", chunk);
}
