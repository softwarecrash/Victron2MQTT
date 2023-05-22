# VeDirectFrameHandler

Arduino library to read from Victron devices using VE.Direct protocol.

This library is based on a publically released reference implementation by Victron. Victron only released the framehandler.cpp portion, so some assumptions were made to adapt it to use for Arduino.
The VE.Direct Protocol FAQ is located here: https://www.victronenergy.com/live/vedirect_protocol:faq

The library was tested using a NodeMCU-1.0.  Doing this requires using SoftwareSerial to connect to the VE device in order to use the hardware UART for debug purposes.
If your platform has 2 hardware UARTs, use them, not SoftwareSerial.

The application passes serial bytes to the library.  The library parses those bytes, verifies the frame, and makes a public buffer available to the application to read as a name/value pair.

