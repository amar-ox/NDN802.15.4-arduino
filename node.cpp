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

#include "node.h"

void Node::processEvents()
{
  radio_.readPacket();
  
  if ( radio_.getResponse().isAvailable() ) /* TODO: isNdnPacket() */
  {    
    /* if Interest */
    if ( radio_.getResponse().getData()[0] == INTEREST_TYPE )
    {
      //digitalWrite(12, HIGH);
      NdnPacket* data;
      if (prefix_)
      {
        if ( prefix_->rawMatch(radio_.getResponse().getData() + P_NAME_VALUE_INDEX, radio_.getResponse().getData()[P_NAME_LENGTH_INDEX] ) )
        {
          /* local, only for onInterest */
          NdnPacket interest(radio_.getResponse().getData(), radio_.getResponse().getDataLength());
          onInterest_(interest);
          return;
        }
      }
      else if ( (data = cs_.rawMatch(radio_.getResponse().getData() + P_NAME_VALUE_INDEX, radio_.getResponse().getData()[P_NAME_LENGTH_INDEX])) )
      {
        /* TODO: defer Data */
        putData(*data);
      }
      else
      {
        /* global, fwd and keep in the PIT*/
        NdnPacket* interest = new NdnPacket(radio_.getResponse().getData(), radio_.getResponse().getDataLength());
        forwardInterest(interest);
      }
      return;
    }

    /* if Data */
    if ( radio_.getResponse().getData()[0] == DATA_TYPE )
    {
      NdnPacket data(radio_.getResponse().getData(), radio_.getResponse().getDataLength());      
      
    }
  }

  /* TODO: don't call each time. Only every 1s */
  processInterestTimeout();
}


void Node::forwardInterest(NdnPacket* interestCopy)
{
  uint8_t newPitEntryId = pit_.add(interestCopy, 0, 0);
  if ( ! newPitEntryId ) { delete interestCopy; return; }

  float senderCost = interestCopy->getCost();
  uint8_t fibEntryId = 0;
  float myCost = fib_.findEntry(interestCopy, &fibEntryId);
  
  if (myCost < 0)    // FIB full
    return;
            
  float delta = 0;
  if (senderCost == 0.)       // ater timeout or first communication:  senderCost -> infty
    delta =  (float) (MAX_DELTA - myCost);
  else                            // other cases
    delta =  (float) (senderCost - myCost);

  if (delta < 0)                 // cannot do better
    return;

  float theta = TH - fib_.computeNa();
  delta+=theta;
  unsigned int delay = 0;
  if ( delta == MAX_DELTA )
  {
    delay = computeDelayRandom();
  }
  else
  {
    delay = computeDelayPhi(delta);
  }
  
  interestCopy->setCost(myCost);  
  
  if (! radio_.send(interestCopy->get(), interestCopy->getLength(), delay))
  {
    pit_.remove(newPitEntryId);
    fib_.Id++;
    return;
  }
  pit_.attachFibEntry(newPitEntryId, fibEntryId);
  //digitalWrite(12, LOW);
}

void Node::forwardData(NdnPacket& data)
{
  uint8_t pendingInterestId = pit_.find(&data);
  float myCost = fib_.updateCost(&data);
  if ( pendingInterestId )
  {
    data.setCost(myCost);
    putData(data);
    pit_.remove(pendingInterestId);
    cs_.add(data);    
  }
  else
    fib_.Du++;
}

bool Node::putData(NdnPacket& data)
{
  if (data.getLength() > 100)
    return false;
  
  radio_.send(data.get(), data.getLength(), 0);
  return true;
}

void Node::processInterestTimeout()
{
  for ( uint8_t i = 0; i < pit_.getSize(); i++ )
    if ( (millis() - pit_[i]->getTimeMilliseconds()) >= ROUTING_LIFETIME_MS )
    {      
      fib_.resetEntry(pit_[i]->getAttachedFib());
      /* TODO: call onTimeout if exists before removing */
      pit_.remove(pit_[i]->getId());     
    }  
}

/*void Node::compressAndPutData(Data& data)
{
  // TODO: improve
  data[1]-=COMMON_PREFIX_LENGTH;
  data[3]-=COMMON_PREFIX_LENGTH;
  memcpy(data+4, data+4+COMMON_PREFIX_LENGTH, dataLength-COMMON_PREFIX_LENGTH);

  radio_.send(data.get(), data.getLength(), 0);
}*/
