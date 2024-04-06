
#include <Arduino.h>
#include "VeDirectFrameHandler.h"

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
	if ((inbyte == ':') && (mState != CHECKSUM))
	{
		mState = RECORD_HEX;
	}
	if (mState != RECORD_HEX)
	{
		mChecksum += inbyte;
	}
	inbyte = toupper(inbyte);

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
		mChecksum = 0;
		mState = IDLE;
		frameEndEvent(valid);
		break;
	}
	case RECORD_HEX:
		if (hexRxEvent(inbyte))
		{
			//hexFrameCallback();
			mChecksum = 0;
			mState = IDLE;
		}
		break;
	}
}

/*
 * textRxEvent
 * This function is called every time a new name/value is successfully parsed.  It writes the values to the temporary buffer.
 */
void VeDirectFrameHandler::textRxEvent(char *mName, char *mValue)
{
	strcpy(tempName[frameIndex], mName);   // copy name to temporary buffer
	strcpy(tempValue[frameIndex], mValue); // copy value to temporary buffer
	frameIndex++;
}

/*
 *  frameEndEvent
 *  This function is called at the end of the received frame.  If the checksum is valid, the temp buffer is read line by line.
 *  If the name exists in the public buffer, the new value is copied to the public buffer.  If not, a new name/value entry
 *  is created in the public buffer.
 */
void VeDirectFrameHandler::frameEndEvent(bool valid)
{
	if (valid)
	{
		for (int i = 0; i < frameIndex; i++)
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
		Serial.println("\nCRC OK");
		veErrorCount = 0;
		requestCallback(); // call the callback to do other things with the new data
	} else {
		veErrorCount++;
		Serial.println("\nCRC wrong");
	}
	frameIndex = 0; // reset frame
	veError = veErrorCount < veErrorTol ? false : true; //set the crc error flag
}

bool VeDirectFrameHandler::hexRxEvent(uint8_t inbyte)
{
	return inbyte == '\n';
}

void VeDirectFrameHandler::callback(std::function<void()> func) // callback function when finnish request
{
	requestCallback = func;
}
