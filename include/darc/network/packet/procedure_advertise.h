/*
 * Copyright (c) 2012, Prevas A/S
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
 * DARC Procedure Advertise Packet
 *
 * \author Morten Kjaergaard
 */

#include <darc/network/packet/header.h>
#include <darc/network/packet/parser.h>

namespace darc
{
namespace network
{
namespace packet
{

struct ProcedureAdvertise
{
  std::string procedure_name;
  darc::ID procedure_id;
  std::string argument_type_name;
  std::string feedback_type_name;
  std::string result_type_name;

  ProcedureAdvertise()
  {
  }

  size_t read( const uint8_t * data, size_t data_len )
  {
    size_t count = Parser::readString(procedure_name, data, data_len);
    count += Parser::readString(argument_type_name, data + count, data_len - count);
    count += Parser::readString(feedback_type_name, data + count, data_len - count);
    count += Parser::readString(result_type_name, data + count, data_len - count);
    return count
  }

  size_t write( uint8_t * data, size_t size )
  {
    size_t count = Parser::writeString(procedure_name, data, size);
    count += Parser::writeString(procedure_name, data + count, size - count);
    count += Parser::writeString(procedure_name, data + count, size - count);
    count += Parser::writeString(procedure_name, data + count, size - count);
    return count;
  }

};

} // namespace packet
} // namespace network
} // namespace darc

#endif
