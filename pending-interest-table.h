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

#ifndef PENDING_INTEREST_TABLE_HPP
#define PENDING_INTEREST_TABLE_HPP

#include "Arduino.h"
#include "ndn.h"

#define MAX_PIT_ENTRIES 25
#define ROUTING_LIFETIME_MS 1900


typedef void (*OnData)(const NdnPacket& data, const NdnPacket& interest);

typedef void (*OnTimeout)(const NdnPacket& interest);

typedef void (*OnInterest)( NdnPacket& interest);


class PitEntry 
{
  public:
  
  PitEntry(uint8_t pendingInterestId,  NdnPacket* interest, const OnData& onData, const OnTimeout& onTimeout, unsigned long timeMilliseconds)
  : pendingInterestId_(pendingInterestId), isRemoved_(false), onData_(onData), onTimeout_(onTimeout), timeMilliseconds_(timeMilliseconds), fibEntry_(0)
  { 
    interest_ = interest;
  }

  ~PitEntry()
  {
    if ( interest_ )
      delete interest_;
  }

  uint8_t getId() { return pendingInterestId_; }

  NdnPacket*  getInterest() { return interest_; }

  void setIsRemoved() { isRemoved_ = true; }

  bool getIsRemoved() { return isRemoved_; }

  const OnData& getOnData() { return onData_; }

  const OnTimeout& getOnTimeout() { return onTimeout_; }

  unsigned long getTimeMilliseconds() { return timeMilliseconds_; }

  void attachToFib(uint8_t e) { fibEntry_ = e; }

  uint8_t getAttachedFib(){ return fibEntry_; }

  private:    
  NdnPacket* interest_;
  OnData onData_;
  OnTimeout onTimeout_;
  unsigned long timeMilliseconds_;
  uint8_t fibEntry_;
  const uint8_t pendingInterestId_;
  bool isRemoved_;
};


class PendingInterestTable {

public:
PendingInterestTable() : pit_size_(0) {}
~PendingInterestTable()
{
  delete[] entries_;
}
uint8_t add(NdnPacket* interest, const OnData& onData, const OnTimeout& onTimeout);
bool remove(uint8_t pendingInterestId);
uint8_t find(NdnPacket* interest);
uint8_t getNextPendingInterestId();
uint8_t getSize() { return pit_size_; }
bool isFull() { return (pit_size_ == MAX_PIT_ENTRIES); }
PitEntry* operator[](uint8_t i);
void attachFibEntry(uint8_t pitEntryId, uint8_t fibEntryId);
private:
uint8_t nextPendingInterestId_ = 0;
uint8_t pit_size_;
PitEntry* entries_[MAX_PIT_ENTRIES];
};

#endif