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

#include "content-store.h"

void ContentStore::add(NdnPacket& data)
{
  cs_index_ = (cs_index_ + 1) % MAX_CS_ENTRIES; 
  NdnPacket* dataCopy_ = new NdnPacket(data.get(), data.getLength());
  entries_[cs_index_] = dataCopy_;
}

NdnPacket* ContentStore::match(NdnPacket* interest)
{
  for ( int8_t i = 0; i <= cs_index_; i++ )  	
    if ( entries_[i]->nameEquals(interest) )          
      return entries_[i];
  return 0;  
}

NdnPacket* ContentStore::rawMatch(const uint8_t* nameEncoding, uint8_t nameEncodingLength)
{
  for ( int8_t i = 0; i <= cs_index_; i++ )  	
    if ( entries_[i]->rawNameEquals(nameEncoding, nameEncodingLength) )          
      return entries_[i];
  return 0;
}

NdnPacket* ContentStore::operator[](uint8_t i)
{
	return entries_[i];
}
