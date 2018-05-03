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

#ifndef FORWARDING_INTEREST_BASE_HPP
#define FORWARDING_INTEREST_BASE_HPP

#include "Arduino.h"
#include "ndn.h"

#define MAX_FIB_ENTRIES 10
#define ALPHA 0.8
#define TH 0.75
#define MAX_DELTA 7


class FibEntry 
{
  public:
  FibEntry(uint8_t fibEntryId, Name* prefix, float heardCost)
  : fibEntryId_(fibEntryId), prefix_(prefix), minHeardCost_(heardCost) 
  {    
    cost_ = 0.;
    cost_ = (1. - ALPHA) * cost_ + ALPHA * (1 +  heardCost);    
  }

  ~FibEntry() { if ( prefix_ ) delete prefix_; }  

  void updateCost(float heardCost)
  {    
    if ( minHeardCost_ > heardCost )
      minHeardCost_ = heardCost;
    cost_ = (1. - ALPHA) * cost_ + ALPHA * (1 +  minHeardCost_);    
  }  
  
  void reset()
  { 
    minHeardCost_ = MAX_DELTA;
    cost_ = 0.;  // means infty
  }

  uint8_t getId() { return fibEntryId_; }
  float getCost() { return cost_; }
  Name* getPrefix() { return prefix_; }

  private:
  const uint8_t fibEntryId_;
  Name* prefix_;
  float cost_;
  float minHeardCost_;
};


class ForwardingInterestBase 
{
  public:
  ForwardingInterestBase() : fib_size_(0) {}
  ~ForwardingInterestBase() { delete[] entries_; }
  int8_t add(Name* prefix, float heardCost);
  int8_t updateCost(NdnPacket* data);
  float findEntry(NdnPacket* interest, uint8_t* index);
  bool match(NdnPacket* packet, int8_t* index);
  bool find(Name* prefix, int8_t* index);
  int8_t getSize() { return fib_size_; };
  float computeNa();
  FibEntry* operator[](uint8_t i);
  uint8_t getNextFibEntryId();
  void resetEntry(uint8_t entryId);

  uint8_t Du = 0; /* # unsolicited Data */ 
  uint8_t Id = 0; /* # dropped Interests */

  private:
  uint8_t nextFibEntryId_ = 0;
  int8_t fib_size_;
  FibEntry* entries_[MAX_FIB_ENTRIES];  
};

#endif