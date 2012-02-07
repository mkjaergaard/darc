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
 * DARC Parser class
 *
 * \author Morten Kjaergaard
 */

#ifndef __DARC_PACKET_PARSER_H_INCLUDED__
#define __DARC_PACKET_PARSER_H_INCLUDED__

namespace darc
{
namespace network
{
namespace packet
{

class Parser
{
public:
  static size_t readString(std::string& result, const uint8_t * data, size_t len)
  {
    result = std::string((char*)data);
    return result.length() + 1;
  }

  static size_t writeString(const std::string& string, uint8_t * data, size_t len)
  {
    strcpy( (char*)data, string.c_str() );
    return string.length() + 1;
  }

  static size_t readUint32(uint32_t& result, uint8_t * data, size_t len)
  {
    result = ((uint32_t)data[0]) +
            (((uint32_t)data[1]) << 8) +
            (((uint32_t)data[2]) << 16) +
            (((uint32_t)data[3]) << 24);
    return 4;
  }

  static size_t writeUint32(uint32_t value, uint8_t * data, size_t len)
  {
    data[0] =  value        % 0xFF;
    data[1] = (value >> 8)  % 0xFF;
    data[2] = (value >> 16) % 0xFF;
    data[3] = (value >> 24) % 0xFF;
    return 4;
  }

  static size_t readUint64(uint64_t& result, uint8_t * data, size_t len)
  {
    result = ((uint64_t)data[0]) +
            (((uint64_t)data[1]) << 8) +
            (((uint64_t)data[2]) << 16) +
            (((uint64_t)data[3]) << 24) +
            (((uint64_t)data[4]) << 32) +
            (((uint64_t)data[5]) << 40) +
            (((uint64_t)data[6]) << 48) +
            (((uint64_t)data[7]) << 56);
    return 8;
  }

  static size_t writeUint64(uint64_t value, uint8_t * data, size_t len)
  {
    data[0] =  value        & 0xFF;
    data[1] = (value >> 8)  & 0xFF;
    data[2] = (value >> 16) & 0xFF;
    data[3] = (value >> 24) & 0xFF;
    data[4] = (value >> 32) & 0xFF;
    data[5] = (value >> 40) & 0xFF;
    data[6] = (value >> 48) & 0xFF;
    data[7] = (value >> 56) & 0xFF;
    return 8;
  }

};

} // namespace packet
} // namespace network
} // namespace darc

#endif
