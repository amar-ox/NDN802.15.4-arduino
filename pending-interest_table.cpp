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

#include "pending-interest-table.h"

uint8_t PendingInterestTable::add(NdnPacket* interest, const OnData& onData, const OnTimeout& onTimeout)
{
  if ( find(interest) )
    return 0;

  if ( pit_size_ == MAX_PIT_ENTRIES )
  	return 0;  

  PitEntry* e = new PitEntry(getNextPendingInterestId(), interest, onData, onTimeout, millis());
  entries_[pit_size_] = e;
  pit_size_++;
  return e->getId();
}


bool PendingInterestTable::remove(uint8_t pendingInterestId)
{
	if ( !pit_size_ )
	  return false;

	uint8_t i = 0;
	bool stop = false;
	while ( !stop && (i < pit_size_) )
  {
		if ( entries_[i]->getId() == pendingInterestId )
    {
      delete entries_[i];
			stop = true;
		}
    else
    {
			i++;
		}
	}  
	if ( i == pit_size_ )
	  return false;

    pit_size_--;
    if ( i != pit_size_ )
      for ( uint8_t j = i; j < pit_size_; j++ )
	      entries_[j] = entries_[j+1];
    return true;
}

uint8_t PendingInterestTable::find(NdnPacket* packet)
{  
  uint8_t pendingEntryId = 0;
  for ( uint8_t i = 0; (i < pit_size_) && !pendingEntryId; i++ )
    if ( packet->nameEquals(entries_[i]->getInterest()) )
      pendingEntryId = entries_[i]->getId();
  return pendingEntryId;
}

PitEntry* PendingInterestTable::operator[](uint8_t i)
{
	return entries_[i];
}

/* 0 never used. Start from 1 */
uint8_t PendingInterestTable::getNextPendingInterestId()
{
  nextPendingInterestId_++;
  if (nextPendingInterestId_ >= 255)
    nextPendingInterestId_ = 1;
  return nextPendingInterestId_;
}

void PendingInterestTable::attachFibEntry(uint8_t pitEntryId, uint8_t fibEntryId)
{
  for ( int8_t i = 0; i < pit_size_; i++ )
    if ( entries_[i]->getId() ==  pitEntryId)
    {
      entries_[i]->attachToFib(fibEntryId);
      return;
    }
}
