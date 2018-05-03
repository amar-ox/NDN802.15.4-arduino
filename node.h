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

#ifndef NODE_HPP
#define NODE_HPP

#include "Arduino.h"
#include "math.h"
#include "HardwareSerial.h"

#include "ndn.h"
#include "pending-interest-table.h"
#include "forwarding-interest-base.h"
#include "ndn-802154.h"
#include "content-store.h"


/* TODO: compression & no compression mode */
// #define SEGMENT_OFFSET (17+13)
// #define COMMON_PREFIX_LENGTH 13
// #define CONTENT_OFFSET (21+13)
// #define CONTENT_SIZE 20
// #define SIGNATURE_OFFSET 64
// #define SIGNATURE_END_OFFSET 62

#define INTEREST_TYPE 0x05
#define DATA_TYPE 0x06

#define DEFER_SLOT_TIME 0.028 // in ms
#define DW 1023

#define M 9.
#define S 3.5

class Node 
{
  public:
  /* node's constructor */
  Node() {}

  /* process packet events (new Interest/Data, timeout, etc.) */
  void processEvents();

  /* issue an (object) Interest with a callback method */
  //void expressInterest(const NdnPacket& interest);
  
  /* issue an (encoded) Interest with a callback method */
  //void expressInterest(uint8_t* encodedInterest, uint8_t interestLength);

  /* forward Interest with iLNFS strategy */
  void forwardInterest(NdnPacket* interestCopy);
  
  /* */
  void forwardData(NdnPacket& data);

  /* send back a Data packet */
  bool putData(NdnPacket& data);  

  /* set up serial communication of the radio */
  void begin(Stream &serial) 
  { 
    radio_.begin(serial); 
  }

  /* set up a frefix to handle if local producer is present */
  void setPrefix(Name& prefix, const OnInterest& onInterest) 
  { 
    prefix_ = &prefix;
    onInterest_ = onInterest;
  }  

  ~Node(){}

  private:
  /* iLNFS stuff: compute deterministic delay time */
  unsigned int computeDelayPhi(float a)
  {
    unsigned int delay = (unsigned int) (M / exp(a / 2.) + S) ;    
    return delay;
  }

  /* iLNFS stuff: compute random delay time */
  unsigned int computeDelayRandom()
  {
    unsigned int delay = (unsigned int) ((DW + random(0, DW)) * DEFER_SLOT_TIME);    
    return delay;
  }

  /* handle pending Interest timeout */
  void processInterestTimeout();

  /* compress Data packet before broadcast it */
  // void compressAndPutData(Data& data);

  Name* prefix_ = 0;
  PendingInterestTable pit_ = PendingInterestTable ();
  ForwardingInterestBase fib_ = ForwardingInterestBase ();
  ContentStore cs_ = ContentStore ();
  IEEE802154 radio_ = IEEE802154 ();
  OnData onData_;
  OnInterest onInterest_;
};

#endif