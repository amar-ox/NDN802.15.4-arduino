/**
 * Copyright (c) 2018 Amar Abane (a_abane@hotmail.fr). All rights reserved.
 *
 * This file is part of NDN802.15.4.
 *
 * NDN802.15.4 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * NDN802.15.4 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 */

#ifndef NDN802154
#define NDN802154

#include "Arduino.h"
#include "HardwareSerial.h"
#include <inttypes.h>

#define ATAP 2

#define START_BYTE 0x7e
#define ESCAPE 0x7d
#define XON 0x11
#define XOFF 0x13
#define MAX_FRAME_SIZE 110
#define MAX_FRAME_PAYLOAD_SIZE 100
#define BROADCAST_ADDRESS 0xffff
#define TX_16_API_LENGTH 3
#define PACKET_OVERHEAD_LENGTH 6
#define API_ID_INDEX 3
#define DEFAULT_FRAME_ID 0x00
#define DISABLE_ACK_OPTION 0x01
#define TX_16_REQUEST 0x01
#define RX_16_RESPONSE 0x81
#define XBEE_WAIT_TIMEOUT 0xff
#define NO_ERROR 0
#define CHECKSUM_FAILURE 1
#define PACKET_EXCEEDS_BYTE_ARRAY_LENGTH 2
#define UNEXPECTED_START_BYTE 3

#if __cplusplus >= 201103L
#define CONSTEXPR constexpr
#else
#define CONSTEXPR
#endif


class IEEE802154Response {
public:
	IEEE802154Response();
	bool isAvailable();
	void setAvailable(bool complete);
	bool isError();
	uint8_t getErrorCode();
	void setErrorCode(uint8_t errorCode);
	uint8_t getMsbLength();
	void setMsbLength(uint8_t msbLength);
	uint8_t getLsbLength();
	void setLsbLength(uint8_t lsbLength);
	uint8_t getApiId();
	void setApiId(uint8_t apiId);
	uint16_t getPacketLength();
	uint8_t getChecksum();
	void setChecksum(uint8_t checksum);
	void setFrameLength(uint8_t frameLength);
	uint8_t* getFrameData();
	void setFrameData(uint8_t* frameDataPtr);
	uint8_t getFrameDataLength();
	uint8_t* getData();
	uint8_t getDataLength();
	uint8_t getDataOffset();
	void reset();
	void init();

protected:
	uint8_t* _frameDataPtr;

private:
	uint8_t _apiId;
	uint8_t _msbLength;
	uint8_t _lsbLength;
	uint8_t _checksum;
	uint8_t _frameLength;
	bool _complete;
	uint8_t _errorCode;
	uint8_t _frameId;
};


class IEEE802154Request {
public:
	IEEE802154Request(const uint8_t *payload, uint8_t payloadLength);
	uint8_t getFrameData(uint8_t pos);
	uint8_t getFrameDataLength();
	uint8_t getPayloadLength();
	const uint8_t* getPayload();
	void setPayloadLength(uint8_t payloadLength);	

private:
	const uint8_t* _payloadPtr;
	uint8_t _payloadLength;
};


class IEEE802154 {
public:
	IEEE802154();
	void readPacket();
	bool readPacket(unsigned int timeout);	
	void begin(Stream &serial);
	IEEE802154Response& getResponse();
	bool send(uint8_t* payload, uint8_t payloadLength, unsigned int delta_t);
    Stream* _serial;

private:
	bool available();
	uint8_t read();
	void write(uint8_t val);
	void sendByte(uint8_t b, bool escape);
	void resetResponse();
	IEEE802154Response _response;
	bool _escape;
	uint8_t _pos;
	uint8_t b;
	uint8_t _checksumTotal;
	uint8_t _nextFrameId;
	uint8_t _responseFrameData[MAX_FRAME_SIZE];
	
};

#endif //NDN80215
