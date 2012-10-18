#pragma once

#include <darc/id.hpp>

namespace darc
{

struct message_packet
{
  const static uint32_t payload_id = 0x02;

  darc::ID tag_id;

  message_packet() :
    tag_id(ID::create())
  {
  }

  message_packet(const darc::ID& tag_id) :
    tag_id(tag_id)
  {
  }

  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
    ar & tag_id;
  }

};

}
