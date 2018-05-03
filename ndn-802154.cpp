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

#include "ndn-802154.h"

IEEE802154::IEEE802154(): _response(IEEE802154Response()) {
        _pos = 0;
        _escape = false;
        _checksumTotal = 0;
        _nextFrameId = 0;
        _response.init();
        _response.setFrameData(_responseFrameData);
        

#if defined(__AVR_ATmega32U4__) || (defined(TEENSYDUINO) && (defined(KINETISK) || defined(KINETISL)))
        _serial = &Serial1;
#else
        _serial = &Serial;
#endif
}

void IEEE802154::resetResponse() {
	_pos = 0;
	_escape = false;
	_checksumTotal = 0;
	_response.reset();
}

void IEEE802154::begin(Stream &serial) {
	_serial = &serial;
}

bool IEEE802154::available() {
	return _serial->available();
}

uint8_t IEEE802154::read() {
	return _serial->read();
}

void IEEE802154::write(uint8_t val) {
	_serial->write(val);
}

IEEE802154Response& IEEE802154::getResponse() {
	return _response;
}


bool IEEE802154::readPacket(unsigned int timeout) {
	unsigned long start = millis();

    while ( (unsigned int)(millis() - start) < timeout ) 
    {
     	readPacket();
     	if (getResponse().isAvailable())
     	  return true;
     	else if (getResponse().isError())
     	  return false;
    }
    return false;
}

void IEEE802154::readPacket() {	
	if (_response.isAvailable() || _response.isError()) {		
		resetResponse();
	}
    while (available()) {
        b = read();
        if (_pos > 0 && b == START_BYTE && ATAP == 2) {        	
        	_response.setErrorCode(UNEXPECTED_START_BYTE);
        	return;
        }
		if (_pos > 0 && b == ESCAPE) {
			if (available()) {
				b = read();
				b = 0x20 ^ b;
			} else {			
				_escape = true;
				continue;
			}
		}
		if (_escape == true) {
			b = 0x20 ^ b;
			_escape = false;
		}		
		if (_pos >= API_ID_INDEX) {
			_checksumTotal+= b;
		}
        switch(_pos) {
			case 0:
		        if (b == START_BYTE) {
		        	_pos++;
		        }
		        break;
			case 1:				
				_response.setMsbLength(b);
				_pos++;
				break;
			case 2:				
				_response.setLsbLength(b);
				_pos++;
				break;
			case 3:
				_response.setApiId(b);
				_pos++;
				break;
			default:				
				if (_pos > MAX_FRAME_SIZE) {					
					_response.setErrorCode(PACKET_EXCEEDS_BYTE_ARRAY_LENGTH);
					return;
				}
				if (_pos == (_response.getPacketLength() + 3)) {			
					if ((_checksumTotal & 0xff) == 0xff) {
						_response.setChecksum(b);
						_response.setAvailable(true);
						_response.setErrorCode(NO_ERROR);
					} else {						
						_response.setErrorCode(CHECKSUM_FAILURE);
					}					
					_response.setFrameLength(_pos - 4);
					_pos = 0;
					return;
				} else {					
					_response.getFrameData()[_pos - 4] = b;
					_pos++;
				}
        }
    }
}

bool IEEE802154::send(uint8_t* payload, uint8_t payloadLength, unsigned int delta_t) 
{
	IEEE802154Request request = IEEE802154Request(payload, payloadLength);
	uint8_t msbLen = ((request.getFrameDataLength() + 2) >> 8) & 0xff;
	uint8_t lsbLen = (request.getFrameDataLength() + 2) & 0xff;
	uint8_t checksum = 0;
	checksum+= TX_16_REQUEST;
	checksum+= DEFAULT_FRAME_ID;
    for (uint8_t i = 0; i < request.getFrameDataLength(); i++)
	  checksum+= request.getFrameData(i);
	checksum = 0xff - checksum;
    if ( readPacket(delta_t) )
      if ( ! memcmp(payload+3, getResponse().getData()+3, getResponse().getData()[3]+1) )
      {
        memcpy(payload+(payloadLength-4), getResponse().getData()+(getResponse().getDataLength()-4), 4);
        return false;
      }
	sendByte(START_BYTE, false);
	sendByte(msbLen, false);
	sendByte(lsbLen, false);
	sendByte(TX_16_REQUEST, false);
	sendByte(DEFAULT_FRAME_ID, false);
	for (uint8_t i = 0; i < request.getFrameDataLength(); i++)
	  sendByte(request.getFrameData(i), false);
	sendByte(checksum, false);
	return true;
}

void IEEE802154::sendByte(uint8_t b, bool escape) {
	if (escape && (b == START_BYTE || b == ESCAPE || b == XON || b == XOFF)) {
		write(ESCAPE);
		write(b ^ 0x20);
	} else {
		write(b);
	}
}

IEEE802154Response::IEEE802154Response() {}

bool IEEE802154Response::isAvailable() {
	return _complete;
}

void IEEE802154Response::setAvailable(bool complete) {
	_complete = complete;
}

bool IEEE802154Response::isError() {
	return _errorCode > 0;
}

uint8_t IEEE802154Response::getErrorCode() {
	return _errorCode;
}

void IEEE802154Response::setErrorCode(uint8_t errorCode) {
	_errorCode = errorCode;
}

uint8_t IEEE802154Response::getMsbLength() {
	return _msbLength;
}

void IEEE802154Response::setMsbLength(uint8_t msbLength) {
	_msbLength = msbLength;
}

uint8_t IEEE802154Response::getLsbLength() {
	return _lsbLength;
}

void IEEE802154Response::setLsbLength(uint8_t lsbLength) {
	_lsbLength = lsbLength;
}

uint8_t IEEE802154Response::getApiId() {
	return _apiId;
}

void IEEE802154Response::setApiId(uint8_t apiId) {
	_apiId = apiId;
}

uint16_t IEEE802154Response::getPacketLength() {
	return ((_msbLength << 8) & 0xff) + (_lsbLength & 0xff);
}

uint8_t IEEE802154Response::getChecksum() {
	return _checksum;
}

void IEEE802154Response::setChecksum(uint8_t checksum) {
	_checksum = checksum;
}

void IEEE802154Response::setFrameLength(uint8_t frameLength) {
	_frameLength = frameLength;
}

uint8_t* IEEE802154Response::getFrameData() {
	return _frameDataPtr;
}

void IEEE802154Response::setFrameData(uint8_t* frameDataPtr) {
	_frameDataPtr = frameDataPtr;
}

uint8_t IEEE802154Response::getFrameDataLength() {
	return _frameLength;
}

uint8_t* IEEE802154Response::getData() {
	return _frameDataPtr + 4;
}

uint8_t IEEE802154Response::getDataLength() {
	return getPacketLength() - getDataOffset() - 1;
}

uint8_t IEEE802154Response::getDataOffset() {
	return 4;
}

void IEEE802154Response::init() {
	_complete = false;
	_errorCode = NO_ERROR;
	_checksum = 0;
}

void IEEE802154Response::reset() {
	init();
	_apiId = 0;
	_msbLength = 0;
	_lsbLength = 0;
	_frameLength = 0;
}


IEEE802154Request::IEEE802154Request(const uint8_t* payload, uint8_t payloadLength) {
	_payloadPtr = payload;
	_payloadLength = payloadLength;
}

const uint8_t* IEEE802154Request::getPayload() {
	return _payloadPtr;
}

/*void IEEE802154Request::setPayload(uint8_t* payload) {
	_payloadPtr = payload;
}*/

uint8_t IEEE802154Request::getPayloadLength() {
	return _payloadLength;
}

void IEEE802154Request::setPayloadLength(uint8_t payloadLength) {
	_payloadLength = payloadLength;
}

uint8_t IEEE802154Request::getFrameDataLength() {
	return TX_16_API_LENGTH + getPayloadLength();
}

uint8_t IEEE802154Request::getFrameData(uint8_t pos) {

	if (pos == 0) {
		return (BROADCAST_ADDRESS >> 8) & 0xff;
	} else if (pos == 1) {
		return BROADCAST_ADDRESS & 0xff;
	} else if (pos == 2) {
		return DISABLE_ACK_OPTION;
	} else {
		return getPayload()[pos - TX_16_API_LENGTH];
	}
}