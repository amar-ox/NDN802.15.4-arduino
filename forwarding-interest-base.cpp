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

#include "forwarding-interest-base.h"

int8_t ForwardingInterestBase::add(Name* prefix, float heardCost)
{
  if ( fib_size_ == MAX_FIB_ENTRIES )
  	return -1;

  int8_t entryIndex = 0;
  if ( find(prefix, &entryIndex) )
    return entryIndex;

  FibEntry* e = new FibEntry(getNextFibEntryId(), prefix, heardCost);
  entries_[fib_size_] = e;
  fib_size_++;
  return (fib_size_ - 1);
}


int8_t ForwardingInterestBase::updateCost(NdnPacket* data)
{
  int8_t entryIndex = -1;
  if ( match(data, &entryIndex) )
  {
    entries_[entryIndex]->updateCost(data->getCost());
    return entryIndex;
  }

  if ( fib_size_ == MAX_FIB_ENTRIES )
    return -1;

  Name* prefix = new Name(data->getPrefix(), data->getPrefixLength());
  FibEntry* e = new FibEntry(getNextFibEntryId(), prefix, data->getCost());
  entries_[fib_size_] = e;
  fib_size_++;
  return (fib_size_ - 1);
}

float ForwardingInterestBase::findEntry(NdnPacket* interest, uint8_t* id)
{
  for ( int8_t i = 0; i < fib_size_; i++ )
    if ( entries_[i]->getPrefix()->match(interest) )
    {
      *id = entries_[i]->getId();
      return entries_[i]->getCost();
    }

  if ( fib_size_ == MAX_FIB_ENTRIES )
    return -1;
  return 0;
}

bool ForwardingInterestBase::match(NdnPacket* data, int8_t* index)
{
  for ( int8_t i = 0; i < fib_size_; i++ )
    if ( entries_[i]->getPrefix()->match(data) )
    {
      *index = i;
      return true;
    }
  return false;
}

bool ForwardingInterestBase::find(Name* prefix, int8_t* index)
{
  for ( int8_t i = 0; i < fib_size_; i++ )
    if ( entries_[i]->getPrefix()->equals(prefix) )
    {
      *index = i;
      return true;
    }
  return false;
}

float ForwardingInterestBase::computeNa()
{
  if (Id == 0)
    return TH;

  float Na = (float) Du / (float) Id;
  if (Na > 1)
    Na = 1;      
  return Na;  
}

FibEntry* ForwardingInterestBase::operator[](uint8_t i)
{
	return entries_[i];
}

/* 0 never used. Start from 1 */
uint8_t ForwardingInterestBase::getNextFibEntryId()
{
  nextFibEntryId_++;
  if (nextFibEntryId_ >= 255)
    nextFibEntryId_ = 1;
  return nextFibEntryId_;
}

void ForwardingInterestBase::resetEntry(uint8_t entryId)
{
  for ( int8_t i = 0; i < fib_size_; i++ )
    if ( entries_[i]->getId() == entryId )
    {
      entries_[i]->reset();
      return;
    } 
}