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

#ifndef CONTENT_STORE_HPP
#define CONTENT_STORE_HPP

#include "Arduino.h"
#include "ndn.h"

#define MAX_CS_ENTRIES 5


class ContentStore 
{
  public:
  ContentStore()
  {
  }

  ~ContentStore()
  {
    delete[] entries_;
  }
 
  void add(NdnPacket& data);

  NdnPacket* match(NdnPacket* interest);
  NdnPacket* rawMatch(const uint8_t* nameEncoding, uint8_t nameEncodingLength);

  NdnPacket* operator[](uint8_t i);

  private:
  int8_t cs_index_ = -1;  
  NdnPacket* entries_[MAX_CS_ENTRIES];
};

#endif