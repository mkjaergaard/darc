/*
 * Copyright (c) 2011, Prevas A/S
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Prevas A/S nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * DARC Header class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PACKET_HEADER_H_INCLUDED__
#define __DARC_PACKET_HEADER_H_INCLUDED__

namespace darc
{
namespace packet
{

struct Header
{
  typedef enum {NONE = 0x00, CONTROL_PACKET = 0x01, MSG_PACKET = 0x02} PayloadType;

  uint32_t sender_node_id;
  PayloadType payload_type; //store as uint8
  uint8_t reserved8;
  uint16_t reserved16;

  Header( uint32_t node_id, PayloadType payload_type):
    sender_node_id(node_id),
    payload_type(payload_type),
    reserved8(0),
    reserved16(0)
  {
  }

  Header():
    sender_node_id(0xFFFFFFFF),
    payload_type(NONE),
    reserved8(0),
    reserved16(0)
  {
  }

  size_t read( const uint8_t * data, size_t len )
  {
    sender_node_id = (uint32_t)data[0] + ((uint32_t)data[1] << 8) + ((uint32_t)data[2] << 16) + ((uint32_t)data[3] << 24);
    payload_type = (PayloadType)data[4];
    return size();
  }

  size_t write( uint8_t * data, size_t len )
  {
    data[0] =  sender_node_id        % 0xFF;
    data[1] = (sender_node_id >> 8)  % 0xFF;
    data[2] = (sender_node_id >> 16) % 0xFF;
    data[3] = (sender_node_id >> 24) % 0xFF;
    data[4] = (uint8_t) payload_type;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    return size();
  }

  static size_t size()
  {
    return 8;
  }

};

} // namespace packet
} // namespace darc

#endif
