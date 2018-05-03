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


#ifndef NDN_HPP
#define NDN_HPP

#include "Arduino.h"
#include <SD.h>

#define LENGTH_INDEX 1
#define VALUE_INDEX 2

#define FLOAT_SIZE 4

#define P_NAME_LENGTH_INDEX 3
#define P_NAME_VALUE_INDEX 4

#define P_PREFIX_OFFSET 4
#define P_PREFIX_LENGTH 14

#define MAX_NAME_LENGTH 30
#define MAX_PACKET_LENGTH 60

#define DATA_ALL "dataAll.txt"

static void log(char code[], float value)
{
  File logRouting_ = SD.open(DATA_ALL, FILE_WRITE);
  if ( logRouting_ )
  {  
    logRouting_.print(millis());logRouting_.print(':');logRouting_.print(code);logRouting_.print('|');logRouting_.println(value);
    logRouting_.close();
  }
}

class NdnPacket 
{
  public:
  
  /* creates the Iterest object and copies the encoding */
  NdnPacket(uint8_t* encoding, uint8_t encodingLength): packetLength_(encodingLength) 
  {
    memcpy(packet_, encoding, encodingLength);
    COST_OFFSET = encodingLength - FLOAT_SIZE;
  }

  ~NdnPacket() { /*if (interest_) delete interest_;*/ }
  
  /* returns a pointer to the whole interest starting from the Type byte */
  uint8_t* get() { return packet_; }

  /* returns the total length including Type & Length bytes */
  uint8_t getLength() { return packetLength_; }

  /* returns a pointer to the first byte of the Value */ 
  const uint8_t* getValue() { return  (packet_ + VALUE_INDEX); }

  /* returns the length of the value */ 
  uint8_t getValueLength() { return packet_[LENGTH_INDEX]; }

  /* returns the length of the name */
  uint8_t getNameLength() { return packet_[P_NAME_LENGTH_INDEX]; }

  /* returns a pointer to the first Value byte of the name */
  const uint8_t* getName() { return  (packet_ + P_NAME_VALUE_INDEX); }

  /* returns the length of the prefix */
  uint8_t getPrefixLength() { return P_PREFIX_LENGTH; }

  /* returns a pointer to the first Value byte of the prefix */
  const uint8_t* getPrefix() { return  (packet_ + P_PREFIX_OFFSET); }

  float getCost()
  {
    float ctg;
    memcpy(&ctg, packet_+COST_OFFSET, FLOAT_SIZE);
    return ctg;
  }

  void setCost(float ctg)
  {
    memcpy(packet_+COST_OFFSET, &ctg, FLOAT_SIZE);
  }

  bool equals(NdnPacket* other)
  {
    if ( packet_[LENGTH_INDEX] != other->getValueLength() )
      return false;
    return (memcmp(packet_ + VALUE_INDEX, other->getValue(), packet_[LENGTH_INDEX]) == 0);  
  }

  bool nameEquals(NdnPacket* other)
  {
    if ( packet_[P_NAME_LENGTH_INDEX] != other->getNameLength() )
      return false;
    return (memcmp(packet_ + P_NAME_VALUE_INDEX, other->getName(), packet_[P_NAME_LENGTH_INDEX]) == 0);  
  }

  bool rawNameEquals(const uint8_t* nameEncoding, uint8_t nameEncodingLength){
    if ( packet_[P_NAME_LENGTH_INDEX] != nameEncodingLength )
      return false;
    return (memcmp(packet_ + P_NAME_VALUE_INDEX, nameEncoding, packet_[P_NAME_LENGTH_INDEX]) == 0);   
  }

  private:
  uint8_t packet_[MAX_PACKET_LENGTH];
  const uint8_t packetLength_;
  uint8_t COST_OFFSET;
};

/*
class Data
{   
  public:
  
  Data(uint8_t* encoding, uint8_t encodingLength): dataLength_(encodingLength) 
  {
    memcpy(data_, encoding, encodingLength);
    GAIN_OFFSET = encodingLength - FLOAT_SIZE;
  }

  ~Data() {}
  
  // returns a pointer to the whole interest starting from the Type byte
  uint8_t* get() { return data_; }

  // returns the total length including Type & Length bytes 
  uint8_t getLength() { return dataLength_; }

  // returns a pointer to the first byte of the Value 
  const uint8_t* getValue() { return  (data_ + VALUE_INDEX); }

  // returns the length of the value  
  uint8_t getValueLength() { return data_[LENGTH_INDEX]; }

  // returns the length of the name 
  uint8_t getNameLength() { return data_[P_NAME_LENGTH_INDEX]; }

  // returns a pointer to the first Value byte of the name 
  const uint8_t* getName() { return  (data_ + P_NAME_VALUE_INDEX); }


  // returns the length of the prefix 
  uint8_t getPrefixLength() { return P_PREFIX_LENGTH; }

  // returns a pointer to the first Value byte of the prefix 
  const uint8_t* getPrefix() { return  (data_ + P_PREFIX_OFFSET); }


  float getCost()
  {
    float gtg;
    memcpy(&gtg, data_+GAIN_OFFSET, FLOAT_SIZE);
    return gtg;
  }

  void setCost(float gtg)
  {
    memcpy(data_+GAIN_OFFSET, &gtg, FLOAT_SIZE);
  }

  bool equals(Data* other)
  {
    if ( data_[LENGTH_INDEX] != other->getValueLength() )
      return false;
    return (memcmp(data_ + VALUE_INDEX, other->getValue(), data_[LENGTH_INDEX]) == 0);  
  }

  bool nameEquals(Data* other)
  {
    if ( data_[P_NAME_LENGTH_INDEX] != other->getNameLength() )
      return false;
    return (memcmp(data_ + P_NAME_VALUE_INDEX, other->getName(), data_[P_NAME_LENGTH_INDEX]) == 0);  
  }

  bool nameEquals(Interest* other)
  {
    if ( data_[P_NAME_LENGTH_INDEX] != other->getNameLength() )
      return false;
    return (memcmp(data_ + P_NAME_VALUE_INDEX, other->getName(), data_[P_NAME_LENGTH_INDEX]) == 0);  
  }


  private:    
  uint8_t data_[MAX_DATA_LENGTH];
  const uint8_t dataLength_;
  uint8_t GAIN_OFFSET;
};  
*/

class Name 
{   
  public:
  
  Name(const uint8_t* encoding, uint8_t encodingLength): nameLength_(encodingLength) 
  {
    memcpy(name_, encoding, encodingLength);
  }

  ~Name() {}

  /* returns a pointer to the whole name starting from the Type */
  const uint8_t* get() { return name_; }

  /* returns the total length including Type & Length */
  uint8_t getLength() { return nameLength_; }

  bool equals(Name* other)
  {
    if ( nameLength_ != other->getLength() )
      return false;
    return ( memcmp(name_, other->get(), nameLength_) == 0 );  
  }

  bool match(NdnPacket* packet_)
  {
    if ( nameLength_ > packet_->getNameLength() )
      return false;
    return ( memcmp(name_, packet_->getName(), nameLength_) == 0 );  
  }

  bool rawMatch(const uint8_t* encoding, uint8_t encodingLength)
  {
    if ( nameLength_ > encodingLength )
      return false;
    return ( memcmp(name_, encoding, nameLength_) == 0 );  
  }

  private:
  uint8_t name_[MAX_NAME_LENGTH];
  const uint8_t nameLength_;
};


#endif